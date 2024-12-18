
#include "conditionnal_basket.h"
#include <corecrt_math.h>

double  ConditionnalBasket::payoff(PnlMat *path, PnlVect *strikes, PnlVect *paymentDates) {
    int nb_assets = path->m;       
    int nb_payoffs = strikes->size;

    double sum = 0;
  
    for (int m = 0; m < nb_payoffs; m++) {
        double t_m = GET(paymentDates, m);
        int t_m_idx = (int)t_m;
        double sum_asset_prices = 0.0;
        for (int n = 0; n < nb_assets; n++) {
            sum_asset_prices += MGET(path, n, t_m_idx);
        }
        double average_price = sum_asset_prices / nb_assets;
        double K_m = GET(strikes, m);

        double P_m = (average_price - K_m) > 0 ? (average_price - K_m):0;
        sum += P_m * exp(interestRate*(GET(paymentDates,paymentDates->size-1)-GET(paymentDates,m)));
       
      
    }
    return sum;
}
