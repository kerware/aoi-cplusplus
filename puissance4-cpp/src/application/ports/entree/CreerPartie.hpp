// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#pragma once

#include "domaine/Partie.hpp"

#include <string>

namespace p4::application {

struct DemandeCreationPartie {
    std::string nomJoueur1;
    std::string nomJoueur2;
};

// PORT D'ENTRÉE (driving port) : ce que l'extérieur peut demander au domaine.
// Appelé par l'adaptateur REST ; implémenté par ServicePartie.
class CreerPartie {
public:
    virtual ~CreerPartie() = default;

    // Lève std::invalid_argument si la demande est invalide.
    virtual domaine::Partie creer(const DemandeCreationPartie& demande) = 0;
};

} // namespace p4::application
