#pragma once

#include "option_multiflux.h"

class ConditionnalMax : public OptionMultiflux {
public:
    ConditionnalMax(PnlVect *strikes, PnlVect *paymentDates)
        : OptionMultiflux(strikes, paymentDates) {}

    double payoff(PnlMat *path, double interestRate) override;
};
