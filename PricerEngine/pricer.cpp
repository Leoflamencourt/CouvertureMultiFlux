#include <iostream>
#include "pnl/pnl_random.h"
#include "json_reader.hpp"
#include "pricer.hpp"

BlackScholesPricer::BlackScholesPricer(nlohmann::json &jsonParams) {
    jsonParams.at("VolCholeskyLines").get_to(volatility);
    jsonParams.at("MathPaymentDates").get_to(paymentDates);
    jsonParams.at("Strikes").get_to(strikes);
    jsonParams.at("DomesticInterestRate").get_to(interestRate);
    jsonParams.at("RelativeFiniteDifferenceStep").get_to(fdStep);
    jsonParams.at("SampleNb").get_to(nSamples);
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
    int index = std::floor(currentDate / timeStep); // Assuming timeStep is the granularity of time steps

    PnlVect *lastRow = pnl_vect_create(past->n);
    pnl_mat_get_row(lastRow, past, past->m - 1);

    // If the current date is a monitoring date, set the next index for simulation
    int startIndex = isMonitoringDate ? index + 1 : index;

    // Temporary vectors for diffusion term
    PnlVect *gaussian = pnl_vect_create(past->n);
    PnlVect *diffusionTerm = pnl_vect_create(past->n);

    for (int i = startIndex; i < paymentDates->size; i++) {
        double t = paymentDates->array[i - 1]; 
        double t_next = paymentDates->array[i]; 

        double dt = t_next - t; // Time step
        double sqrtDt = std::sqrt(dt);

        // Generate correlated Gaussian variables
        for (int d = 0; d < past->n; d++) {
            pnl_vect_set(gaussian, d, pnl_rand_normal(generator)); // Independent normal variables
        }
        pnl_mat_mult_vect(diffusionTerm, volatility, gaussian); // Correlated diffusion

        for (int d = 0; d < past->n; d++) { // For each dimension
            double drift = (interestRate - 0.5 * pnl_mat_get(volatility, d, d) * pnl_mat_get(volatility, d, d)) * dt;
            double diffusion = pnl_vect_get(diffusionTerm, d) * sqrtDt;
            double S_t = pnl_vect_get(lastRow, d); // Last known value
            double S_tNext = S_t * exp(drift + diffusion);

            pnl_mat_set(path, i, d, S_tNext); 
        }

        pnl_mat_get_row(lastRow, path, i);
    }

    pnl_vect_free(&lastRow);
    pnl_vect_free(&gaussian);
    pnl_vect_free(&diffusionTerm);
}



void BlackScholesPricer::montecarlo(double& price, double& priceStdDev, PnlMat* path) {

}

void BlackScholesPricer::priceAndDeltas(const PnlMat *past, double currentDate, bool isMonitoringDate, double &price, double &priceStdDev, PnlVect* &deltas, PnlVect* &deltasStdDev) {
    //std::pair<double, double> pricing_results = montecarlo(nSamples, currentDate);
    price = 0.0;
    priceStdDev = 0.0;
    deltas = pnl_vect_create_from_zero(nAssets);
    deltasStdDev = pnl_vect_create_from_zero(nAssets);
    /* A compléter */

    PnlMat* path = pnl_mat_create_from_zero(paymentDates->size, nAssets);
    



}
