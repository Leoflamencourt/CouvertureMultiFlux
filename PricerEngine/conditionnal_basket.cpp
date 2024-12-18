#include "conditionnal_basket.h"

void ConditionnalBasket::payoff(PnlMat* path, PnlVect* strikes, PnlVect* paymentDates)
{
    //TODO:Implement the payoff
    int nb_assets = path->m;
    int nb_payoffs = strikes->size;
    for (int n = 0; n < nb_assets; n++) {
        for (int m = 0; m < nb_payoffs; m++) {
        }
    }
}
