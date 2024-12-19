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

    // Determine the index of the current date in the paymentDates array
    int index = past->n - 1; // Assuming timeStep is the granularity of time steps

    PnlVect *lastRow = pnl_vect_create(nAssets);
    pnl_mat_get_col(lastRow, past, past->n - 1);
    
    
    // If the current date is a monitoring date, set the next index for simulation
    int startIndex = isMonitoringDate ? index + 1 : index;

   

    double t = currentDate;

    for (int i = startIndex; i < paymentDates->size+1; i++) { 
        double t_next = GET(paymentDates, i-1); 

        double dt = t_next - t; 
        double sqrtDt = std::sqrt(dt);

        generateCorrelatedShocks();

        for (int d = 0; d <nAssets; d++) { // For each asset
            PnlMat *volatility_t = pnl_mat_create(1, 1);
            volatility_t = pnl_mat_copy(volatility);
            volatility_t = pnl_mat_transpose(volatility_t);
            double sigma2 = MGET(pnl_mat_mult_mat(volatility, volatility_t), d, d);
           
            double drift = (interestRate - 0.5 * sigma2)* dt;
            PnlVect *LD = pnl_vect_create(0);
            pnl_mat_get_row(LD, volatility, d);
         
            double diffusion = pnl_vect_scalar_prod(LD, mIndependentShocks )* sqrtDt;

            double S_t = pnl_vect_get(lastRow, d); 
            double S_tNext = S_t * exp(drift + diffusion);
            
            pnl_mat_set(path, d, i, S_tNext); 
            
        }
      
        
        pnl_mat_get_col(lastRow, path, i);
      
        t = t_next;
    }
    pnl_vect_free(&lastRow);
   
}

void BlackScholesPricer::montecarlo(const PnlMat *past, double currentDate, bool isMonitoringDate, double &price, double &priceStdDev, PnlVect *deltas, PnlVect *deltasStdDev) {
    double runningSum = 0;
    double runningSquaredSum = 0;

    PnlMat *path = pnl_mat_create(nAssets, paymentDates->size+1);

    // Initialisez les vecteurs pour les sommes et les sommes quadratiques des deltas
    pnl_vect_resize(deltas, nAssets);
    pnl_vect_resize(deltasStdDev, nAssets);
    pnl_vect_set_all(deltas, 0.0);
    PnlVect *runningDeltaSquaredSum = pnl_vect_create_from_zero(nAssets);

    // Simulez plusieurs chemins et calculez payoffs + deltas
    for (unsigned long i = 0; i < nSamples; i++) {
        // Génération du chemin standard
        asset(past, currentDate, isMonitoringDate, path);

        // Calcul du payoff standard
        double payoff = option_multiflux->payoff(path, interestRate);
       
        runningSum += payoff;
        /*std::cout << payoff << std::endl;
        pnl_mat_print(path);*/
        runningSquaredSum += payoff * payoff;

        PnlMat *pathPlus = pnl_mat_copy(path);
        PnlMat *pathMinus = pnl_mat_copy(path);

        // Calcul des deltas par différences finies pour chaque actif
        for (int j = 0; j < nAssets; j++) {
            // Perturber le prix de l'actif j
            perturbAssetPrice(pathPlus, fdStep, true, j, past->n - 1);
            perturbAssetPrice(pathMinus, fdStep, false, j, past->n - 1);

            // Payoffs avec perturbations
            double payoffPlus = option_multiflux->payoff(pathPlus, interestRate);
            double payoffMinus = option_multiflux->payoff(pathMinus, interestRate);

            // Contribution au delta pour l'actif j
            double deltaContribution = (payoffPlus - payoffMinus) / (2 * MGET(past, j, past->n - 1) * fdStep);
            LET(deltas, j) += deltaContribution;

            // Mise à jour de la somme des carrés des deltas pour l'écart-type
            double deltaSquaredContribution = deltaContribution * deltaContribution;
            LET(runningDeltaSquaredSum, j) = deltaSquaredContribution;
        }

        pnl_mat_free(&pathPlus);
        pnl_mat_free(&pathMinus);
    }

    double maturity = GET(paymentDates, paymentDates->size - 1);

    // Discount the average payoff to the present value
    price = exp(-1 * interestRate * (maturity - currentDate)) * runningSum / nSamples;

    // Calculate the variance and standard deviation for the price
    double variance = (runningSquaredSum / nSamples) - (runningSum / nSamples) * (runningSum / nSamples);
    priceStdDev = exp(-2 * interestRate * (maturity - currentDate))* sqrt(variance / nSamples);

    // Moyenne et actualisation des deltas + calcul de l'écart-type
    for (int j = 0; j < nAssets; j++) {
        // Moyenne et actualisation des deltas
        LET(deltas, j) *= exp(-1 * interestRate * (maturity - currentDate)) / nSamples;

        // Variance pour le delta
        double meanDelta = LET(deltas, j);
        double deltaVariance = (GET(runningDeltaSquaredSum, j) / nSamples) - meanDelta * meanDelta;

        // Écart-type pour le delta
        LET(deltasStdDev, j) =   sqrt(deltaVariance / nSamples);
    }

    pnl_mat_free(&path);
    pnl_vect_free(&runningDeltaSquaredSum);
}


void BlackScholesPricer::perturbAssetPrice(PnlMat *path, double fdStep, bool isUp, int assetIndex, int lastIndex) {
    // Définir le facteur de perturbation
    double perturbFactor = isUp ? (1 + fdStep) : (1 - fdStep);

    // Perturber uniquement l'actif spécifié (ligne assetIndex) à partir de la colonne lastIndex
    for (int col = lastIndex; col < path->n; ++col) {
        MLET(path, assetIndex, col) *= perturbFactor; // Multiplier chaque élément par le facteur
    }
}

void BlackScholesPricer::priceAndDeltas(const PnlMat *past, double currentDate, bool isMonitoringDate, double &price, double &priceStdDev, PnlVect *&deltas, PnlVect *&deltasStdDev) {


    
    montecarlo(past, currentDate, isMonitoringDate, price, priceStdDev, deltas, deltasStdDev);


    
}
