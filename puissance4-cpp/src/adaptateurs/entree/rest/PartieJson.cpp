// Dernière génération/modification faite par l'IA Claude le 09/10/2026 15:56:12
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
        {"nombreDeCoups", partie.nombreDeCoups()},
        {"vainqueur", partie.vainqueur() ? nlohmann::json(partie.vainqueur()->nom()) : nlohmann::json(nullptr)},
        {"dernierCoup", partie.dernierCoup()
                            ? nlohmann::json{{"colonne", partie.dernierCoup()->colonne},
                                             {"ligne", partie.dernierCoup()->ligne},
                                             {"couleur", std::string(domaine::versTexte(partie.dernierCoup()->couleur))},
                                             {"joueur", partie.dernierCoup()->joueur}}
                            : nlohmann::json(nullptr)},
    };
}

} // namespace p4::adaptateurs
