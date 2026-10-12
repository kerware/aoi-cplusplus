// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#include "domaine/Grille.hpp"

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

bool Grille::estVide() const {
    for (const auto& ligne : cases_) {
        for (Jeton jeton : ligne) {
            if (jeton != Jeton::Vide) {
                return false;
            }
        }
    }
    return true;
}

} // namespace p4::domaine
