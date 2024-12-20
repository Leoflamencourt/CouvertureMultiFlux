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

 
}

void BlackScholesPricer::asset(const PnlMat *past, double currentDate, bool isMonitoringDate, PnlMat *path) {

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
            pnl_mat_free(&volatility_t);
            
        }
      

        pnl_mat_get_col(lastRow, path, i);
      
        t = t_next;
    }
    pnl_vect_free(&lastRow);
   
}

void BlackScholesPricer::montecarlo(const PnlMat *past, double currentDate, bool isMonitoringDate, double &price, double &priceStdDev, PnlVect *deltas, PnlVect *deltasStdDev) {
    double runningSum = 0;
    double runningSquaredSum = 0;

    PnlMat *path = pnl_mat_create(nAssets, paymentDates->size + 1);

    // Initialisez les vecteurs pour les sommes et les sommes quadratiques des deltas
   


    double payoffPlus, payoffMinus;
    double T = GET(paymentDates, paymentDates->size - 1);

    for (int i = 0; i < nSamples; ++i) {
        // Génération du chemin standard
        asset(past, currentDate, isMonitoringDate, path);
      
        // Calcul du payoff standard
        double payoff = option_multiflux->payoff(path, interestRate);
        runningSum += payoff;
        runningSquaredSum += payoff * payoff;
      
   
        // Clonage des chemins pour le calcul des deltas
        PnlMat *pathPlus = pnl_mat_copy(path);
        PnlMat *pathMinus = pnl_mat_copy(path);
       
        for (int assetIndex = 0; assetIndex < nAssets; ++assetIndex) {
            // Perturber le chemin pour l'actif courant
            perturbAssetPrice(pathPlus, fdStep, true, assetIndex, past->n - 1);
            perturbAssetPrice(pathMinus, fdStep, false, assetIndex, past->n - 1);

            // Calcul des payoffs pour les chemins perturbés
            payoffPlus = option_multiflux->payoff(pathPlus, interestRate);
            payoffMinus = option_multiflux->payoff(pathMinus, interestRate);

            // Calcul du delta contribution
            double deltaContribution = (payoffPlus - payoffMinus) / (2 * fdStep * MGET(past, assetIndex, past->n - 1));
            double deltaSquaredContribution = deltaContribution * deltaContribution;
            
            LET(deltas, assetIndex) += deltaContribution;
            LET(deltasStdDev, assetIndex) += deltaSquaredContribution;

            // Réinitialiser les chemins pour le prochain actif
            pnl_mat_clone(pathPlus, path);
            pnl_mat_clone(pathMinus, path);
        }

        pnl_mat_free(&pathPlus);
        pnl_mat_free(&pathMinus);
    }

    // Calcul du prix moyen et de son écart-type
    price = exp(-interestRate * (T - currentDate)) * runningSum / nSamples;
    double variance = (runningSquaredSum / nSamples) - (runningSum / nSamples) * (runningSum / nSamples);
    priceStdDev = exp(-2 * interestRate * (T - currentDate)) * sqrt(abs(variance / nSamples));

    // Moyenne et actualisation des deltas
    pnl_vect_mult_scalar(deltas, 1.0 / nSamples);
 
    pnl_vect_mult_scalar(deltasStdDev, 1.0 / nSamples);
    PnlVect *deltaMeanSquared = pnl_vect_copy(deltas);
    pnl_vect_mult_vect_term(deltaMeanSquared, deltaMeanSquared);



   
    pnl_vect_minus_vect(deltasStdDev, deltaMeanSquared);
    
    
    for (int i = 0; i < nAssets; ++i) {
        LET(deltasStdDev, i) = exp(-2 * interestRate * (T - currentDate)) *sqrt(abs(GET(deltasStdDev, i)/nSamples));

    }
    pnl_vect_mult_scalar(deltas, exp(-interestRate * (T - currentDate)));

    pnl_mat_free(&path);
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
    price = 0.;
    priceStdDev = 0.;
    deltas = pnl_vect_create_from_zero(nAssets);
    deltasStdDev = pnl_vect_create_from_zero(nAssets);


    
    montecarlo(past, currentDate, isMonitoringDate, price, priceStdDev, deltas, deltasStdDev);


    
}
