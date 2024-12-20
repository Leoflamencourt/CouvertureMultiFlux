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

   PnlMat *pastInvented = pnl_mat_create(blackScholesPricer.nAssets, 1);

    // Initialisation des valeurs de la première colonne
    MLET(pastInvented, 0, 0) = 17; // Actif 0, colonne 0
    MLET(pastInvented, 1, 0) = 20; // Actif 1, colonne 0
    MLET(pastInvented, 2, 0) = 15; // Actif 2, colonne 0
    MLET(pastInvented, 3, 0) = 14; // Actif 3, colonne 0
    MLET(pastInvented, 4, 0) = 13; 


    //// Initialisation des valeurs de la première colonne
    //MLET(pastInvented, 0, 1) = 13,3; // Actif 0, colonne 0
    //MLET(pastInvented, 1, 1) = 15; // Actif 1, colonne 0
    //MLET(pastInvented, 2, 1) = 17,13; // Actif 2, colonne 0
    //MLET(pastInvented, 3, 1) =16,89; // Actif 3, colonne 0
    //MLET(pastInvented, 4, 1) = 14,16; // Actif 4, colonne 0
   

    double currentDate = 0;
   // blackScholesPricer.asset(pastInvented, currentDate, false, path);   

    double price;
    double stddev;

    PnlVect *deltas = pnl_vect_create(blackScholesPricer.nAssets);
    PnlVect *detlasStddev = pnl_vect_create(blackScholesPricer.nAssets);

    blackScholesPricer.priceAndDeltas(pastInvented, currentDate, true, price, stddev, deltas, detlasStddev);
    blackScholesPricer.print();

    std::cout << "Overall results for priceAndDeltas call : " << std::endl;
    std::cout << "The price : " << price << std::endl;
    std::cout << "The stddev : " << stddev << std::endl;
    std::cout << "Deltas:" << std::endl;
    
    for (int i = 0; i < deltas->size; i++) {
        std::cout << GET(deltas, i) << "\t";
    }
    std::cout << "Deltas Stddev:" << std::endl;
    for (int i = 0; i < deltas->size; i++) {
        std::cout << GET(detlasStddev, i) << "\t";
    }
    exit(0);
}

