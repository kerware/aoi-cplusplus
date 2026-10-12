// Dernière génération/modification faite par l'IA Claude le 09/10/2026 15:55:30
#pragma once

#include "domaine/Grille.hpp"
#include "domaine/Joueur.hpp"

#include <optional>
#include <string>
#include <string_view>

namespace p4::domaine {

using IdPartie = std::string;

enum class StatutPartie { EnCours, Gagnee };

std::string_view versTexte(StatutPartie statut);

// Trace d'un coup joué (EX-11).
struct Coup {
    int colonne;
    int ligne;
    Jeton couleur;
    std::string joueur;
};

// Une partie oppose deux joueurs : joueur 1 = ROUGE (commence), joueur 2 = JAUNE.
class Partie {
public:
    // Lève std::invalid_argument si l'identifiant est vide ou si les noms sont invalides.
    Partie(IdPartie id, const std::string& nomJoueur1, const std::string& nomJoueur2);

    // Vérifie les noms SANS créer de partie (EX-02, EX-05) : chaque nom est valide
    // et les deux noms sont différents (casse ASCII et espaces de bord ignorés).
    static void verifierNoms(const std::string& nomJoueur1, const std::string& nomJoueur2);

    // Itération 2 : le joueur courant joue dans la colonne (EX-06),
    // puis la main passe à l'adversaire (EX-07).
    // Lève CoupInvalide ou ColonnePleine ; dans ce cas la partie est inchangée.
    Coup jouer(int colonne);

    const IdPartie& id() const { return id_; }
    const Joueur& joueur1() const { return joueur1_; }
    const Joueur& joueur2() const { return joueur2_; }
    const Joueur& joueurCourant() const { return tourDuJoueur1_ ? joueur1_ : joueur2_; }
    const Grille& grille() const { return grille_; }
    StatutPartie statut() const { return statut_; }
    int nombreDeCoups() const { return nombreDeCoups_; }
    const std::optional<Coup>& dernierCoup() const { return dernierCoup_; }
    const std::optional<Joueur>& vainqueur() const { return vainqueur_; }

private:
    IdPartie id_;
    Joueur joueur1_;
    Joueur joueur2_;
    Grille grille_;
    StatutPartie statut_ = StatutPartie::EnCours;
    bool tourDuJoueur1_ = true;
    int nombreDeCoups_ = 0;
    std::optional<Coup> dernierCoup_;
    std::optional<Joueur> vainqueur_;
};

} // namespace p4::domaine
