#include "pnl/pnl_vector.h"
#include "pricer.hpp"
#include "pnl/pnl_matrix.h"
#include <iostream>
#include <fstream>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Wrong number of arguments. Exactly one argument is required" << std::endl;
        std::exit(0);
    }
    std::ifstream ifs(argv[1]);
    nlohmann::json j = nlohmann::json::parse(ifs);
    BlackScholesPricer blackScholesPricer = BlackScholesPricer(j);

    PnlMat *pastInvented = pnl_mat_create_from_scalar(blackScholesPricer.nAssets, 1, 100.0);

    PnlMat *path = pnl_mat_create(blackScholesPricer.nAssets, blackScholesPricer.paymentDates->size);

    blackScholesPricer.asset(pastInvented, 0.2, false, path);

    // Print the generated path matrix
    std::cout << "Generated Path Matrix:" << std::endl;
    for (int i = 0; i < path->m; ++i) {     // Iterate over rows (assets)
        for (int j = 0; j < path->n; ++j) { // Iterate over columns (time steps)
            std::cout << pnl_mat_get(path, i, j) << "\t";
        }
        std::cout << std::endl;
    }

    exit(0);
}

