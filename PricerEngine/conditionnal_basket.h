#pragma once

#include "option_multiflux.h"

class ConditionnalBasket : public OptionMultiflux {
    double payoff(PnlMat *path, PnlVect *strikes, PnlVect *paymentDates);
};