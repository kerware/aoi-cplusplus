// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#pragma once

#include "domaine/Partie.hpp"

#include <optional>

namespace p4::application {

// PORT D'ENTRÉE : lecture d'une partie existante.
class ConsulterPartie {
public:
    virtual ~ConsulterPartie() = default;

    // std::nullopt si aucune partie ne porte cet identifiant.
    virtual std::optional<domaine::Partie> consulter(const domaine::IdPartie& id) const = 0;
};

} // namespace p4::application
