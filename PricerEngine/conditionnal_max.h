#pragma once

#include "option.h"

static class ConditionnalMax : public Option {
    void payoff(PnlMat *path, PnlVect *strikes, PnlVect *paymentDates);
};