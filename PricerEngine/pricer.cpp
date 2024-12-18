#include <iostream>
#include "pnl/pnl_random.h"
#include "json_reader.hpp"
#include "pricer.hpp"
#include "vanilla_call.h"
#include "conditionnal_max.h"
#include "conditionnal_basket.h"
#include "option.h"

BlackScholesPricer::BlackScholesPricer(nlohmann::json &jsonParams) {
    jsonParams.at("VolCholeskyLines").get_to(volatility);
    jsonParams.at("MathPaymentDates").get_to(paymentDates);
    jsonParams.at("Strikes").get_to(strikes);
    jsonParams.at("DomesticInterestRate").get_to(interestRate);
    jsonParams.at("RelativeFiniteDifferenceStep").get_to(fdStep);
    jsonParams.at("SampleNb").get_to(nSamples);
    jsonParams.at("PayoffType").get_to(payoffType);
    //option = createOption(payoffType, strikes, paymentDates);
    option = VanillaCall(strikes, paymentDates);
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

    // Determine the index of the current date in the paymentDates array
    int index = past->m - 1; // Assuming timeStep is the granularity of time steps

    PnlVect *lastRow = pnl_vect_create(nAssets);
    pnl_mat_get_row(lastRow, past, past->m - 1);

    // If the current date is a monitoring date, set the next index for simulation
    int startIndex = isMonitoringDate ? index + 1 : index;

    PnlVect *diffusionTerm = pnl_vect_create(past->n);

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
    double payoff = 0;
    for (unsigned long i = 0; i < nSamples; i++) {
        asset(past, currentDate, isMonitoringDate, path);
        
        runningSum += payoff;
        runningSquaredSum += payoff * payoff;
    }
    double maturity = GET(paymentDates,paymentDates->size -1);
    price = exp(-1 * interestRate * (maturity - currentDate)) * runningSum / nSamples;
    double variance = exp(-2 * interestRate * maturity) * runningSquaredSum / nSamples - price * price;
    priceStdDev = 1.96 * sqrt(variance / nSamples);
}



//std::unique_ptr<Option> createOption(const std::string &payoffType, PnlVect *strikes, PnlVect *paymentDates) {
//    if (payoffType == "ConditionalMax") {
//        return std::make_unique<ConditionnalMax>(strikes, paymentDates);
//    } else if (payoffType == "ConditionalBasket") {
//        return std::make_unique<ConditionnalBasket>(strikes, paymentDates);
//    } else if (payoffType == "VanillaCall") {
//        return std::make_unique<VanillaCall>(strikes, paymentDates);
//    } else {
//        throw std::invalid_argument("Unknown payoffType: " + payoffType);
//    }
//}

void BlackScholesPricer::priceAndDeltas(const PnlMat *past, double currentDate, bool isMonitoringDate, double &price, double &priceStdDev, PnlVect* &deltas, PnlVect* &deltasStdDev) {
    price = 0.0;
    priceStdDev = 0.0;
    deltas = pnl_vect_create_from_zero(nAssets);
    deltasStdDev = pnl_vect_create_from_zero(nAssets);
    /* A compléter */
    PnlMat* path = pnl_mat_create_from_zero(paymentDates->size, nAssets);

}
