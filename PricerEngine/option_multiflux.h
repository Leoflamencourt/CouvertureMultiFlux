#pragma once

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

class OptionMultiflux {
public:
    OptionMultiflux(PnlVect *strikes, PnlVect *paymentDates, double interestRate);
    virtual double payoff(PnlMat *path) = 0;
    PnlVect *strikes;
    PnlVect *paymentDates;
    double interestRate;
};