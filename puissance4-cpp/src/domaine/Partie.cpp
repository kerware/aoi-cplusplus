// Dernière génération/modification faite par l'IA Claude le 09/10/2026 16:47:56
#include "domaine/Partie.hpp"

#include "domaine/Erreurs.hpp"

#include <cctype>
#include <algorithm>
#include <stdexcept>
#include <utility>

namespace p4::domaine {

namespace {

// Minuscules ASCII uniquement : "ÉLOÏSE" et "éloïse" restent différents (limite connue).
// std::transform en place.
std::string enMinuscules(std::string texte) {
    std::transform(texte.begin(), texte.end(), texte.begin(), [](char caractere) {
        return static_cast<char>(std::tolower(static_cast<unsigned char>(caractere)));
    });
    return texte;
}

} // namespace

std::string_view versTexte(StatutPartie statut) {
    switch (statut) {
        case StatutPartie::Gagnee: return "GAGNEE";
        case StatutPartie::Nulle: return "NULLE";
        case StatutPartie::EnCours: break;
    }
    return "EN_COURS";
}

void Partie::verifierNoms(const std::string& nomJoueur1, const std::string& nomJoueur2) {
    // noms locaux distincts des méthodes joueur1()/joueur2()
    const Joueur premier(nomJoueur1, Jeton::Rouge);  // lève si le nom est invalide
    const Joueur second(nomJoueur2, Jeton::Jaune);
    if (enMinuscules(premier.nom()) == enMinuscules(second.nom())) {
        throw std::invalid_argument("Les deux joueurs doivent avoir des noms différents");
    }
}

Partie::Partie(IdPartie id, const std::string& nomJoueur1, const std::string& nomJoueur2)
    : id_(std::move(id)),
      joueur1_(nomJoueur1, Jeton::Rouge),
      joueur2_(nomJoueur2, Jeton::Jaune) {
    if (id_.empty()) {
        throw std::invalid_argument("L'identifiant de partie est obligatoire");
    }
    verifierNoms(nomJoueur1, nomJoueur2);
}

Coup Partie::jouer(int colonne) {
    if (statut_ != StatutPartie::EnCours) {
        throw PartieTerminee();  // EX-17
    }
    const Joueur& joueur = joueurCourant();
    // Grille::jouer lève avant toute modification : un coup refusé ne change rien (EX-09).
    const int ligne = grille_.jouer(colonne, joueur.couleur());
    Coup coup{colonne, ligne, joueur.couleur(), joueur.nom()};
    dernierCoup_ = coup;
    ++nombreDeCoups_;
    if (grille_.aligneQuatre(ligne, colonne)) {
        statut_ = StatutPartie::Gagnee;
        vainqueur_ = joueur;
        return coup;
    }
    if (grille_.estPleine()) {  // EX-16 : 42 coups sans alignement
        statut_ = StatutPartie::Nulle;
        return coup;
    }
    tourDuJoueur1_ = !tourDuJoueur1_;
    return coup;
}

} // namespace p4::domaine
