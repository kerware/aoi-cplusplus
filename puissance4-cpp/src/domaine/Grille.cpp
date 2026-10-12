// Dernière génération/modification faite par l'IA Claude le 09/10/2026 15:10:00
#include "domaine/Grille.hpp"

#include "domaine/Erreurs.hpp"

#include <stdexcept>
#include <string>

namespace p4::domaine {

Jeton Grille::caseEn(int ligne, int colonne) const {
    if (ligne < 0 || ligne >= LIGNES || colonne < 0 || colonne >= COLONNES) {
        throw std::out_of_range("Case hors grille : (" + std::to_string(ligne) + ", " +
                                std::to_string(colonne) + ")");
    }
    return cases_[ligne][colonne];
}

bool Grille::estVide() const { return nombreDeJetons() == 0; }

int Grille::nombreDeJetons() const {
    int total = 0;
    for (const auto& ligne : cases_) {
        for (Jeton jeton : ligne) {
            if (jeton != Jeton::Vide) {
                ++total;
            }
        }
    }
    return total;
}

bool Grille::colonnePleine(int colonne) const { return caseEn(0, colonne) != Jeton::Vide; }

int Grille::jouer(int colonne, Jeton jeton) {
    if (jeton == Jeton::Vide) {
        throw std::invalid_argument("On ne joue pas un jeton vide");
    }
    if (colonne < 0 || colonne > COLONNES) {
        throw CoupInvalide(colonne);
    }
    // Le jeton « tombe » : on cherche la case libre la plus basse.
    for (int ligne = LIGNES - 1; ligne >= 0; --ligne) {
        Jeton& caseCourante = cases_.at(ligne).at(colonne);
        if (caseCourante == Jeton::Vide) {
            caseCourante = jeton;
            return ligne;
        }
    }
    throw ColonnePleine(colonne);
}

} // namespace p4::domaine
