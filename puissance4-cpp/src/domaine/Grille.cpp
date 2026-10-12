// Dernière génération/modification faite par l'IA Claude le 09/10/2026 16:11:04
#include "domaine/Grille.hpp"

#include "domaine/Erreurs.hpp"

#include <stdexcept>
#include <string>

namespace p4::domaine {

namespace {

struct Direction {
    int dLigne;
    int dColonne;
};

// Directions examinées pour détecter un alignement (chacune est parcourue dans les deux sens).
// Itération 3 : seule la direction horizontale est gérée pour l'instant.
constexpr std::array<Direction, 1> DIRECTIONS{{{0, 1}}};

} // namespace

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
    // Colonnes valides : 0 à COLONNES - 1 (EX-08)
    if (colonne < 0 || colonne >= COLONNES) {
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

int Grille::compterDansLaDirection(int ligne, int colonne, int dLigne, int dColonne, Jeton jeton) const {
    int total = 0;
    int l = ligne + dLigne;
    int c = colonne + dColonne;
    while (l >= 0 && l < LIGNES && c >= 0 && c < COLONNES && cases_[l][c] == jeton) {
        ++total;
        l += dLigne;
        c += dColonne;
    }
    return total;
}

bool Grille::aligneQuatre(int ligne, int colonne) const {
    const Jeton jeton = caseEn(ligne, colonne);
    if (jeton == Jeton::Vide) {
        return false;
    }
    for (const Direction& d : DIRECTIONS) {
        const int alignes = 1 + compterDansLaDirection(ligne, colonne, d.dLigne, d.dColonne, jeton) +
                            compterDansLaDirection(ligne, colonne, -d.dLigne, -d.dColonne, jeton);
        if (alignes >= 4) {
            return true;
        }
    }
    return false;
}

} // namespace p4::domaine
