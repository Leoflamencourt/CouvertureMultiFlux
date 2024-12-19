#include "payoff.h"
#include "conditionnal_basket.h"
#include "conditionnal_max.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include <iostream>

int main() {

    int nAssets = 3;
    int nPayments = 3;
    double interestRate = 0.05;

    PnlVect *paymentDates = pnl_vect_create(nPayments);
    pnl_vect_set(paymentDates, 0, 1.0);
    pnl_vect_set(paymentDates, 1, 2.0);
    pnl_vect_set(paymentDates, 2, 3.0);

    PnlVect *strikes = pnl_vect_create(nPayments);
    pnl_vect_set(strikes, 0, 100.0);
    pnl_vect_set(strikes, 1, 110.0);
    pnl_vect_set(strikes, 2, 120.0);

    PnlMat *test_path = pnl_mat_create(nAssets, nPayments);

    double assetPrices[3][3] = {
        {105.0, 110.0, 125.0}, // Prices for Asset 1
        {95.0, 100.0, 125.0},  // Prices for Asset 2
        {110.0, 115.0, 125.0}  // Prices for Asset 3
    };

    for (int i = 0; i < nAssets; i++) {
        for (int j = 0; j < nPayments; j++) {
            pnl_mat_set(test_path, i, j, assetPrices[i][j]);
        }
    }

    ConditionnalBasket basketOption = ConditionnalBasket(strikes, paymentDates);

    ConditionnalMax maxOption = ConditionnalMax(strikes, paymentDates);

    double payoff_basket = basketOption.payoff(test_path, interestRate);
    double payoff_max = maxOption.payoff(test_path, interestRate);

    std::cout << "This is the payoff for our simple basketOption: " << payoff_basket << std::endl;
    std::cout << "This is the payoff for our simple maxOption: " << payoff_max << std::endl;

    pnl_vect_free(&paymentDates);
    pnl_vect_free(&strikes);
    pnl_mat_free(&test_path);

    return 0;
}
