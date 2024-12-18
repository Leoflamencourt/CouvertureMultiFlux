#pragma once

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

class Option {
private:
    PnlVect *strikes;
    PnlVect *paymentDates;

public :
    Option(PnlVect *strikes, PnlVect *paymentDates);
    virtual ~Option();
    void virtual payoff(PnlMat *path) = 0;
};