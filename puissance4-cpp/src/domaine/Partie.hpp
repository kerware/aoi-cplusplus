// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#pragma once

#include "domaine/Grille.hpp"
#include "domaine/Joueur.hpp"

#include <string>
#include <string_view>

namespace p4::domaine {

using IdPartie = std::string;

enum class StatutPartie { EnCours };

std::string_view versTexte(StatutPartie statut);

// Une partie oppose deux joueurs : joueur 1 = ROUGE (commence), joueur 2 = JAUNE.
class Partie {
public:
    // Lève std::invalid_argument si l'identifiant est vide
    // (les noms sont validés par Joueur).
    Partie(IdPartie id, const std::string& nomJoueur1, const std::string& nomJoueur2);

    const IdPartie& id() const { return id_; }
    const Joueur& joueur1() const { return joueur1_; }
    const Joueur& joueur2() const { return joueur2_; }
    const Joueur& joueurCourant() const { return joueur1_; }  // itération 1 : personne n'a joué
    const Grille& grille() const { return grille_; }
    StatutPartie statut() const { return statut_; }

private:
    IdPartie id_;
    Joueur joueur1_;
    Joueur joueur2_;
    Grille grille_;
    StatutPartie statut_ = StatutPartie::EnCours;
};

} // namespace p4::domaine
