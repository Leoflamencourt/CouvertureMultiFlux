#pragma once

#include "option.h"

class ConditionnalBasket : public Option {
    void payoff(PnlMat *path, PnlVect *strikes, PnlVect *paymentDates);
};