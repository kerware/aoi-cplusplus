// Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:23:27
#pragma once

#include "domaine/Jeton.hpp"

#include <string>

namespace p4::domaine {

// Un joueur a un nom (1 à 20 caractères, espaces de bord retirés) et une couleur.
class Joueur {
public:
    static constexpr std::size_t LONGUEUR_MAX_NOM = 20;

    // Lève std::invalid_argument si le nom est vide/blanc, trop long,
    // ou si la couleur est Jeton::Vide.
    Joueur(const std::string& nom, Jeton couleur);  // 

    const std::string& nom() const { return nom_; }
    Jeton couleur() const { return couleur_; }

    bool operator==(const Joueur&) const = default;

private:
    std::string nom_;
    Jeton couleur_;
};

} // namespace p4::domaine
