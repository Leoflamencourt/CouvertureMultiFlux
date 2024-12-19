#include <iostream>
#include "pnl/pnl_random.h"
#include "json_reader.hpp"
#include "pricer.hpp"

#include "conditionnal_max.h"
#include "conditionnal_basket.h"
#include "option_multiflux.h"

static OptionMultiflux *createOption(const std::string &payoffType, PnlVect *strikes, PnlVect *paymentDates) {
    if (payoffType == "ConditionalMax") {
        return new ConditionnalMax(strikes, paymentDates);
    } else if (payoffType == "ConditionalBasket") {
        return new ConditionnalBasket(strikes, paymentDates);
    } else {
        throw std::invalid_argument("Unknown payoffType: " + payoffType);
    }
}

BlackScholesPricer::BlackScholesPricer(nlohmann::json &jsonParams) {
    jsonParams.at("VolCholeskyLines").get_to(volatility);
    jsonParams.at("MathPaymentDates").get_to(paymentDates);
    jsonParams.at("Strikes").get_to(strikes);
    jsonParams.at("DomesticInterestRate").get_to(interestRate);
    jsonParams.at("RelativeFiniteDifferenceStep").get_to(fdStep);
    jsonParams.at("SampleNb").get_to(nSamples);
    jsonParams.at("PayoffType").get_to(payoffType);
    
    option_multiflux = createOption(payoffType, strikes, paymentDates);
    nAssets = volatility->n;
    mIndependentShocks = pnl_vect_create(nAssets);
    mCorrelatedShocks = pnl_vect_create(nAssets);
    mRng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(mRng, time(NULL));
}

BlackScholesPricer::~BlackScholesPricer() {
    pnl_vect_free(&paymentDates);
    pnl_vect_free(&strikes);
    pnl_mat_free(&volatility);
    delete option_multiflux;
}

void BlackScholesPricer::print() {
    std::cout << "nAssets: " << nAssets << std::endl;
    std::cout << "fdStep: " << fdStep << std::endl;
    std::cout << "nSamples: " << nSamples << std::endl;
    std::cout << "strikes: ";
    pnl_vect_print_asrow(strikes);
    std::cout << "paymentDates: ";
    pnl_vect_print_asrow(paymentDates);
    std::cout << "volatility: ";
    pnl_mat_print(volatility);
}

void BlackScholesPricer::generateCorrelatedShocks() {
    int D = nAssets;

    pnl_vect_rng_normal(mIndependentShocks, D, mRng);

    // Apply Cholesky decomposition to correlate the shocks
    pnl_mat_mult_vect_inplace(mCorrelatedShocks, volatility, mIndependentShocks);
}

void BlackScholesPricer::asset(const PnlMat *past, double currentDate, bool isMonitoringDate, PnlMat *path) {
    // Copy the past data into the path matrix
    pnl_mat_set_subblock(path, past, 0, 0);

    std::cout << "Generated Path Matrix:" << std::endl;
    for (int i = 0; i < path->m; ++i) {     // Iterate over rows (assets)
        for (int j = 0; j < path->n; ++j) { // Iterate over columns (time steps)
            std::cout << pnl_mat_get(path, i, j) << "\t";
        }
        std::cout << std::endl;
    }

    // Determine the index of the current date in the paymentDates array
    int index = past->n - 1; // Assuming timeStep is the granularity of time steps

    PnlVect *lastRow = pnl_vect_create(nAssets);
    pnl_mat_get_row(lastRow, past, past->n - 1);

    
    // If the current date is a monitoring date, set the next index for simulation
    int startIndex = isMonitoringDate ? index + 1 : index;

    PnlVect *diffusionTerm = pnl_vect_create(past->m);

    double t = currentDate;

    for (int i = startIndex; i < paymentDates->size; i++) { 
        double t_next = GET(paymentDates, i); 

        double dt = t_next - t; 
        double sqrtDt = std::sqrt(dt);

        generateCorrelatedShocks();

        for (int d = 0; d < nAssets; d++) { // For each asset
            double drift = (interestRate - 0.5 * pnl_mat_get(volatility, d, d) * pnl_mat_get(volatility, d, d)) * dt;
            double diffusion = pnl_vect_get(diffusionTerm, d) * sqrtDt;
            double S_t = pnl_vect_get(lastRow, d); 
            double S_tNext = S_t * exp(drift + diffusion);

            pnl_mat_set(path, i, d, S_tNext); 
        }

        pnl_mat_get_row(lastRow, path, i);
        t = t_next;
    }
    pnl_vect_free(&lastRow);
    pnl_vect_free(&diffusionTerm);
}

void BlackScholesPricer::montecarlo(const PnlMat *past, double currentDate, bool isMonitoringDate, double &price, double &priceStdDev, PnlMat *path) {
    double runningSum = 0;
    double runningSquaredSum = 0;

    // Simulate multiple paths and calculate payoffs
    for (unsigned long i = 0; i < nSamples; i++) {
        // Generate a path using the asset method
        asset(past, currentDate, isMonitoringDate, path);

        // Calculate the payoff using the generic option's payoff method
        double payoff = option_multiflux->payoff(path, interestRate);

        runningSum += payoff;
        runningSquaredSum += payoff * payoff;
    }

    double maturity = GET(paymentDates, paymentDates->size - 1);

    // Discount the average payoff to the present value
    price = exp(-1 * interestRate * (maturity - currentDate)) * runningSum / nSamples;

    // Calculate the variance and standard deviation
    double variance = (runningSquaredSum / nSamples) - (runningSum / nSamples) * (runningSum / nSamples);
    priceStdDev = 1.96 * sqrt(variance / nSamples);
}

void BlackScholesPricer::perturbAssetPrice(PnlMat *path, const PnlMat *past, double currentDate, bool isMonitoringDate, int assetIndex, double fdStep, bool isUp) {
    PnlVect *lastRow = pnl_vect_create(nAssets);
    pnl_mat_get_row(lastRow, past, past->m - 1);

    double perturbFactor = isUp ? (1 + fdStep) : (1 - fdStep);
    double perturbedPrice = GET(lastRow, assetIndex) * perturbFactor;
    pnl_vect_set(lastRow, assetIndex, perturbedPrice);

    pnl_mat_set_row(path, lastRow, past->m - 1);

    // Generate future path using the perturbed lastRow
    asset(past, currentDate, isMonitoringDate, path);

    pnl_vect_free(&lastRow);
}

void BlackScholesPricer::priceAndDeltas(const PnlMat *past, double currentDate, bool isMonitoringDate, double &price, double &priceStdDev, PnlVect *&deltas, PnlVect *&deltasStdDev) {
    price = 0.0;
    priceStdDev = 0.0;
    deltas = pnl_vect_create_from_zero(nAssets);
    deltasStdDev = pnl_vect_create_from_zero(nAssets);

    PnlMat *path = pnl_mat_create_from_zero(paymentDates->size, nAssets);
    double basePrice = 0.0, basePriceStdDev = 0.0;

    // Compute the base price
    montecarlo(past, currentDate, isMonitoringDate, basePrice, basePriceStdDev, path);

    // Loop through each asset to calculate deltas using finite differences
    for (int d = 0; d < nAssets; d++) {
        // Perturb the underlying asset price upward
        PnlMat *pathPlus = pnl_mat_copy(path);
        PnlMat *pathMinus = pnl_mat_copy(path);
        perturbAssetPrice(pathPlus, past, currentDate, isMonitoringDate, d, fdStep, true);
        perturbAssetPrice(pathMinus, past, currentDate, isMonitoringDate, d, fdStep, false);

        // Calculate the payoff for perturbed prices
        double pricePlus = 0.0, pricePlusStdDev = 0.0;
        double priceMinus = 0.0, priceMinusStdDev = 0.0;
        montecarlo(past, currentDate, isMonitoringDate, pricePlus, pricePlusStdDev, pathPlus);
        montecarlo(past, currentDate, isMonitoringDate, priceMinus, priceMinusStdDev, pathMinus);

        // Calculate delta using finite differences
        double s = MGET(past, past->m - 1, d); // Current asset price
        double delta = (pricePlus - priceMinus) / (2 * s * fdStep);
        LET(deltas, d) = delta;

        // Cleanup
        pnl_mat_free(&pathPlus);
        pnl_mat_free(&pathMinus);
    }

    // Set final price and standard deviation
    price = basePrice;
    priceStdDev = basePriceStdDev;

    pnl_mat_free(&path);
}
