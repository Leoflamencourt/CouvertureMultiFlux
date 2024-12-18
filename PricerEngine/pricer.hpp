#pragma once

#include <iostream>
#include <nlohmann/json.hpp>
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_random.h"

class BlackScholesPricer {
public:
    PnlMat *volatility;
    PnlVect *paymentDates;
    PnlVect *strikes;
    PnlRng *mRng;
    PnlVect *mIndependentShocks;
    PnlVect *mCorrelatedShocks;
    int nAssets;
    double interestRate;
    double fdStep;
    int nSamples;

    BlackScholesPricer(nlohmann::json &jsonParams);
    ~BlackScholesPricer();
    void generateCorrelatedShocks();
    void asset(const PnlMat *past, double currentDate, bool isMonitoringDate, PnlMat *path);
    void montecarlo(double &price, double &priceStdDev, PnlMat *path);
    void priceAndDeltas(const PnlMat *past, double currentDate, bool isMonitoringDate, double &price, double &priceStdDev, PnlVect* &deltas, PnlVect* &deltasStdDev);
    void print();
};