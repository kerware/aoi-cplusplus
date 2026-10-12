// Dernière génération/modification faite par l'IA Claude le 09/10/2026 15:10:00
#include "domaine/Partie.hpp"

#include <cctype>
#include <stdexcept>
#include <utility>

namespace p4::domaine {

namespace {

// Minuscules ASCII uniquement : "ÉLOÏSE" et "éloïse" restent différents (limite connue).
std::string enMinuscules(std::string texte) {
    for (char& caractere : texte) {
        caractere = static_cast<char>(std::tolower(static_cast<unsigned char>(caractere)));
    }
    return texte;
}

} // namespace

std::string_view versTexte(StatutPartie statut) {
    switch (statut) {
        case StatutPartie::EnCours: break;
    }
    return "EN_COURS";
}

void Partie::verifierNoms(const std::string& nomJoueur1, const std::string& nomJoueur2) {
    const Joueur joueur1(nomJoueur1, Jeton::Rouge);  // lève si le nom est invalide
    const Joueur joueur2(nomJoueur2, Jeton::Jaune);
    if (enMinuscules(joueur1.nom()) == enMinuscules(joueur2.nom())) {
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
    const Joueur& joueur = joueurCourant();
    // Grille::jouer lève avant toute modification : un coup refusé ne change rien (EX-09).
    const int ligne = grille_.jouer(colonne, joueur.couleur());
    Coup coup{colonne, ligne, joueur.couleur(), joueur.nom()};
    dernierCoup_ = coup;
    ++nombreDeCoups_;
    tourDuJoueur1_ = !tourDuJoueur1_;
    return coup;
}

} // namespace p4::domaine
