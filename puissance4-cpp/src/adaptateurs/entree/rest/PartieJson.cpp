// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#include "adaptateurs/entree/rest/PartieJson.hpp"

#include <string>

namespace p4::adaptateurs {

namespace {

nlohmann::json versJson(const domaine::Joueur& joueur) {
    return {{"nom", joueur.nom()}, {"couleur", std::string(domaine::versTexte(joueur.couleur()))}};
}

} // namespace

nlohmann::json versJson(const domaine::Partie& partie) {
    nlohmann::json grille = nlohmann::json::array();
    for (int ligne = 0; ligne < domaine::Grille::LIGNES; ++ligne) {
        std::string texte;
        for (int colonne = 0; colonne < domaine::Grille::COLONNES; ++colonne) {
            texte += domaine::versSymbole(partie.grille().caseEn(ligne, colonne));
        }
        grille.push_back(texte);
    }
    return {
        {"id", partie.id()},
        {"statut", std::string(domaine::versTexte(partie.statut()))},
        {"joueur1", versJson(partie.joueur1())},
        {"joueur2", versJson(partie.joueur2())},
        {"joueurCourant", partie.joueurCourant().nom()},
        {"grille", grille},
    };
}

} // namespace p4::adaptateurs
