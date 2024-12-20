#pragma once

#include <iostream>
#include <nlohmann/json.hpp>
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_random.h"
#include "option.h"
class OptionMultiflux;



class BlackScholesPricer {
public:
    PnlMat *volatility;
    PnlVect *paymentDates;
    PnlVect *strikes;
    PnlRng *mRng;
    PnlVect *mIndependentShocks;
    PnlVect *mCorrelatedShocks;
    std::string payoffType;
    int nAssets;
    double interestRate;
    double fdStep;
    int nSamples;

    OptionMultiflux *option_multiflux;

    BlackScholesPricer(nlohmann::json &jsonParams);
    ~BlackScholesPricer();
    void generateCorrelatedShocks();
    void asset(const PnlMat *past, double currentDate, bool isMonitoringDate, PnlMat *path);
    void montecarlo(const PnlMat *past, double currentDate, bool isMonitoringDate, double &price, double &priceStdDev, PnlVect *deltas, PnlVect *deltasStdDev);
    void perturbAssetPrice(PnlMat *path, double fdStep, bool isUp, int assetIndex, int lastIndex);
    void priceAndDeltas(const PnlMat *past, double currentDate, bool isMonitoringDate, double &price, double &priceStdDev, PnlVect* &deltas, PnlVect* &deltasStdDev);
    

    void print();
};