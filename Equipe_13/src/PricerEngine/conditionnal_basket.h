#pragma once

#include "option_multiflux.h"

class ConditionnalBasket : public OptionMultiflux {
public:
    ConditionnalBasket(PnlVect *strikes, PnlVect *paymentDates)
        : OptionMultiflux(strikes, paymentDates) {}

    double payoff(PnlMat *path, double interestRate) override; 
};