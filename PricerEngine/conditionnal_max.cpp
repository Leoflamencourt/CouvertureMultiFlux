
#include "conditionnal_max.h"
#include <corecrt_math.h>

double ConditionnalMax::payoff(PnlMat *path, PnlVect *strikes, PnlVect *paymentDates) {
    int nb_assets = path->m;
    int nb_payoffs = strikes->size;
    
    PnlVect *assets = pnl_vect_create(nb_assets);
    double maxAsset = INT_MIN;
    double sum = 0;
    for (int m = 0; m < nb_payoffs; m++) {     
        pnl_mat_get_col(assets, path, m);
        double K_m = GET(strikes, m);
        maxAsset = pnl_vect_max(assets);

        double P_m = (maxAsset - K_m) > 0 ? (maxAsset - K_m) : 0;
        sum += P_m * exp(interestRate * (GET(paymentDates, paymentDates->size - 1) - GET(paymentDates, m)));
    }
    return sum;
}
