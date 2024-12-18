#include "pnl/pnl_vector.h"

#include "vanilla_call.h"

VanillaCall::VanillaCall(PnlVect *strikes, PnlVect *paymentDates)
    : Option(strikes, paymentDates) {
}

double VanillaCall::payoff(PnlMat *path){
    double sum = 0;
    int nAssets = path->n;
    for (int i = 0; i < nAssets; ++i) {
        sum += MGET(path, i, path->m - 1);
    }
    sum /= nAssets;
    return (sum - GET(strikes, 0)) > 0 ? (sum - GET(strikes, 0)) : 0;
}
