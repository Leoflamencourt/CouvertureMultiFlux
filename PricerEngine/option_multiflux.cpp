#include "option_multiflux.h"

OptionMultiflux::OptionMultiflux(PnlVect *strikes, PnlVect *paymentDates, double interestRate)
    : strikes(strikes), paymentDates(paymentDates) {}