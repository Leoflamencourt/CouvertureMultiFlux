#pragma once

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

class Option {
public :
    Option(PnlVect *strikes, PnlVect *paymentDates);
    double virtual payoff(PnlMat *path) = 0;
    PnlVect *strikes;
    PnlVect *paymentDates;
};