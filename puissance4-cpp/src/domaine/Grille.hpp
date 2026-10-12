// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#pragma once

#include "domaine/Jeton.hpp"

#include <array>

namespace p4::domaine {

// Grille de Puissance 4 : 6 lignes x 7 colonnes.
// Convention : ligne 0 = ligne du HAUT, colonne 0 = colonne de GAUCHE.
// Itération 1 : la grille est créée vide (le jeu des coups arrive à l'itération 2).
class Grille {
public:
    static constexpr int LIGNES = 6;
    static constexpr int COLONNES = 7;

    Grille() = default;

    // Lève std::out_of_range si (ligne, colonne) est hors de la grille.
    Jeton caseEn(int ligne, int colonne) const;

    bool estVide() const;

private:
    std::array<std::array<Jeton, COLONNES>, LIGNES> cases_{};  // tout à Jeton::Vide
};

} // namespace p4::domaine
