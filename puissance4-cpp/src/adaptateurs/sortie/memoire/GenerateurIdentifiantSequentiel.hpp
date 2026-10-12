// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#pragma once

#include "application/ports/sortie/GenerateurIdentifiant.hpp"

#include <atomic>

namespace p4::adaptateurs {

// ADAPTATEUR DE SORTIE : identifiants "P0001", "P0002", ... (thread-safe).
class GenerateurIdentifiantSequentiel final : public application::GenerateurIdentifiant {
public:
    domaine::IdPartie suivant() override;

private:
    std::atomic<int> compteur_{0};
};

} // namespace p4::adaptateurs
