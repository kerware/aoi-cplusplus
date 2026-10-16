// Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:23:27
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
    // une seule lecture de l'optional, vérifiée
    nlohmann::json dernierCoup = nullptr;
    if (const auto& coup = partie.dernierCoup(); coup.has_value()) {
        dernierCoup = {{"colonne", coup->colonne},
                       {"ligne", coup->ligne},
                       {"couleur", std::string(domaine::versTexte(coup->couleur))},
                       {"joueur", coup->joueur}};
    }
    nlohmann::json vainqueur = nullptr;
    if (const auto& gagnant = partie.vainqueur(); gagnant.has_value()) {
        vainqueur = gagnant->nom();
    }
    return {
        {"id", partie.id()},
        {"statut", std::string(domaine::versTexte(partie.statut()))},
        {"joueur1", versJson(partie.joueur1())},
        {"joueur2", versJson(partie.joueur2())},
        {"joueurCourant", partie.joueurCourant().nom()},
        {"grille", grille},
        {"nombreDeCoups", partie.nombreDeCoups()},
        {"vainqueur", vainqueur},
        {"dernierCoup", dernierCoup},
    };
}

} // namespace p4::adaptateurs
