#pragma once

#include "option_multiflux.h"

static class ConditionnalMax : public OptionMultiflux {
    double payoff(PnlMat *path, PnlVect *strikes, PnlVect *paymentDates);
};