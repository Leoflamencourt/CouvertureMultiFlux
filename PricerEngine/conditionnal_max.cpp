#include "conditionnal_max.h"
#include <corecrt_math.h>

double ConditionnalMax::payoff(PnlMat *path, double interestRate) {
    int nb_assets = path->m;
    int nb_payoffs = strikes->size;
    int indicatrice= 1;
    
    PnlVect *assets = pnl_vect_create(nb_assets);
    double maxAsset = INT_MIN;
    double sum = 0;
    for (int m = 0; m < nb_payoffs; m++) {     
        pnl_mat_get_col(assets, path, m);
        double K_m = GET(strikes, m);
        maxAsset = pnl_vect_max(assets);
        double P_m = (maxAsset - K_m) > 0 ? (maxAsset - K_m) : 0;

        sum += P_m * indicatrice * exp((GET(paymentDates, paymentDates->size - 1) - GET(paymentDates, m)) * interestRate);
        indicatrice = P_m > 0 ? 0 : 1;
    }
    return sum;
}
