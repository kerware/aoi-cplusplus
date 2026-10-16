// Dernière génération/modification faite par l'IA Claude le 13/10/2026 21:12:10
// État d'un scénario BDD, PARTAGÉ par tous les fichiers d'étapes (bdd/etapes/*.cpp) :
// ctx.obtenir<p4::bdd::Monde>() renvoie le même objet dans toutes les étapes d'un scénario.
// (Un type défini dans un espace de noms anonyme serait différent dans chaque fichier .cpp.)
#pragma once

#include "adaptateurs/sortie/memoire/GenerateurIdentifiantSequentiel.hpp"
#include "adaptateurs/sortie/memoire/PartieRepositoryEnMemoire.hpp"
#include "application/Erreurs.hpp"
#include "application/ServicePartie.hpp"
#include "domaine/Erreurs.hpp"

#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

namespace p4::bdd {

// État d'un scénario : un serveur « en mémoire » neuf pour chaque scénario.
struct Monde {
    p4::adaptateurs::PartieRepositoryEnMemoire repository;
    p4::adaptateurs::GenerateurIdentifiantSequentiel generateur;
    p4::application::ServicePartie service{repository, generateur};

    std::optional<p4::domaine::IdPartie> id;           // la partie du scénario
    std::optional<std::string> refus;              // « colonne pleine », « partie inconnue »…
    std::optional<std::string> message;            // message de l'erreur
    std::optional<p4::domaine::Partie> avantLeCoup;    // pour vérifier « la partie est inchangée »

    p4::domaine::Partie partie() {
        auto p = service.consulter(id.value());
        if (!p) {
            throw std::logic_error("la partie " + *id + " n'existe pas");
        }
        return *p;
    }

    void creer(const std::string& joueur1, const std::string& joueur2) {
        try {
            id = service.creer({joueur1, joueur2}).id();
        } catch (const std::invalid_argument& e) {
            refus = "création";
            message = e.what();
        }
    }

    void jouer(const p4::domaine::IdPartie& partie, int colonne) {
        avantLeCoup = service.consulter(partie);
        refus.reset();
        try {
            service.jouer(partie, colonne);
            return;
        } catch (const p4::domaine::CoupInvalide& e) {
            refus = "colonne invalide";
            message = e.what();
        } catch (const p4::domaine::ColonnePleine& e) {
            refus = "colonne pleine";
            message = e.what();
        } catch (const p4::domaine::PartieTerminee& e) {
            refus = "partie terminée";
            message = e.what();
        } catch (const p4::application::PartieInconnue& e) {
            refus = "partie inconnue";
            message = e.what();
        }
    }

    // « 0 1 0 1 » : coups joués à tour de rôle ; tout refus est une erreur du scénario.
    void jouerSequence(const std::string& sequence) {
        std::istringstream colonnes(sequence);
        for (int colonne; colonnes >> colonne;) {
            service.jouer(id.value(), colonne);
        }
    }
};

} // namespace p4::bdd
