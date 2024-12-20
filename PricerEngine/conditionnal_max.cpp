#include "conditionnal_max.h"
#include <corecrt_math.h>

double ConditionnalMax::payoff(PnlMat *path, double interestRate) {
    int nb_assets = path->m;        // Nombre d'actifs
    int nb_payoffs = strikes->size; // Nombre de paiements
    int indicatrice = 1;            // Indicateur pour stopper le calcul si un paiement est effectué

    PnlVect *assets = pnl_vect_create(nb_assets); // Vecteur pour stocker les prix des actifs à chaque date
    double sum = 0;                               // Somme des paiements actualisés

    for (int m = 1; m < nb_payoffs; m++) {      // Itération sur les dates de paiement
        pnl_mat_get_col(assets, path, m);       // Récupère les prix des actifs à la date m
        double K_m = GET(strikes, m - 1);       // Strike correspondant à la date m
        double maxAsset = pnl_vect_max(assets); // Trouve le maximum des prix des actifs

        // Calcul du paiement pour la date m
        double P_m = (maxAsset - K_m) > 0 ? (maxAsset - K_m) : 0;

        // Si le paiement est positif, on ajoute sa valeur actualisée à la somme
        sum += P_m * indicatrice * exp((GET(paymentDates, paymentDates->size - 1) - GET(paymentDates, m - 1)) * interestRate);

        // Met à jour l'indicatrice : si un paiement a été effectué, on arrête les prochains paiements
        indicatrice = P_m > 0 ? 0 : 1;
    }

    pnl_vect_free(&assets); // Libération de la mémoire pour le vecteur des actifs
    return sum;             // Retourne la somme des paiements actualisés
}
