// Dernière génération/modification faite par l'IA Claude le 09/10/2026 15:10:00
#pragma once

#include "application/ports/entree/ConsulterPartie.hpp"
#include "application/ports/entree/CreerPartie.hpp"
#include "application/ports/entree/JouerCoup.hpp"
#include "application/ports/sortie/GenerateurIdentifiant.hpp"
#include "application/ports/sortie/PartieRepository.hpp"

namespace p4::application {

// Cas d'utilisation : implémente les ports d'entrée
// en s'appuyant uniquement sur les ports de sortie (injectés).
class ServicePartie final : public CreerPartie, public ConsulterPartie, public JouerCoup {
public:
    ServicePartie(PartieRepository& repository, GenerateurIdentifiant& generateur);

    domaine::Partie creer(const DemandeCreationPartie& demande) override;
    std::optional<domaine::Partie> consulter(const domaine::IdPartie& id) const override;
    domaine::Partie jouer(const domaine::IdPartie& id, int colonne) override;

private:
    PartieRepository& repository_;
    GenerateurIdentifiant& generateur_;
};

} // namespace p4::application
