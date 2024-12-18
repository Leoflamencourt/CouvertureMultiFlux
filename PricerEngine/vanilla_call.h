#include "pnl/pnl_vector.h"

#include "option.h"

class VanillaCall : public Option {
public:
    double payoff(PnlMat *path, PnlVect *strikes, PnlVect *paymentDates);
};
