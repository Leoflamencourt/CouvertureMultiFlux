#include "pnl/pnl_vector.h"

#include "option.h"

class VanillaCall : public Option {
public:
    VanillaCall(PnlVect *strikes, PnlVect *paymentDates);
    double payoff(PnlMat *path);
};
