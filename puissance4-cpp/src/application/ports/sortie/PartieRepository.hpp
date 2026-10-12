// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#pragma once

#include "domaine/Partie.hpp"

#include <optional>

namespace p4::application {

// PORT DE SORTIE (driven port) : ce dont le domaine a besoin pour stocker les parties.
// Implémenté par un adaptateur (mémoire aujourd'hui, base de données demain),
// remplacé par un bouchon (mock GoogleMock) dans les tests unitaires.
class PartieRepository {
public:
    virtual ~PartieRepository() = default;

    virtual void sauvegarder(const domaine::Partie& partie) = 0;
    virtual std::optional<domaine::Partie> trouverParId(const domaine::IdPartie& id) const = 0;
};

} // namespace p4::application
