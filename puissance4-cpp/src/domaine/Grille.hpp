// Dernière génération/modification faite par l'IA Claude le 09/10/2026 15:10:00
#pragma once

#include "domaine/Jeton.hpp"

#include <array>

namespace p4::domaine {

// Grille de Puissance 4 : 6 lignes x 7 colonnes.
// Convention : ligne 0 = ligne du HAUT, colonne 0 = colonne de GAUCHE.
class Grille {
public:
    static constexpr int LIGNES = 6;
    static constexpr int COLONNES = 7;

    Grille() = default;

    // Lève std::out_of_range si (ligne, colonne) est hors de la grille.
    Jeton caseEn(int ligne, int colonne) const;

    bool estVide() const;
    int nombreDeJetons() const;

    // Lève std::out_of_range si la colonne n'existe pas.
    bool colonnePleine(int colonne) const;

    // Itération 2 : fait tomber le jeton dans la case libre la plus basse (EX-06).
    // Retourne la ligne où le jeton s'est arrêté.
    // Lève CoupInvalide si la colonne n'existe pas, ColonnePleine si elle est remplie.
    int jouer(int colonne, Jeton jeton);

private:
    std::array<std::array<Jeton, COLONNES>, LIGNES> cases_{};  // tout à Jeton::Vide
};

} // namespace p4::domaine
