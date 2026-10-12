// Dernière génération/modification faite par l'IA Claude le 09/10/2026 15:10:00
#pragma once

#include "domaine/Partie.hpp"

#include <stdexcept>

namespace p4::application {

// Aucune partie ne porte cet identifiant (EX-10). Traduit en HTTP 404.
class PartieInconnue : public std::runtime_error {
public:
    explicit PartieInconnue(const domaine::IdPartie& id)
        : std::runtime_error("Partie inconnue : " + id) {}
};

} // namespace p4::application
