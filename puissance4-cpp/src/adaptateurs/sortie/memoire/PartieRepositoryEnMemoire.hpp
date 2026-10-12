// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#pragma once

#include "application/ports/sortie/PartieRepository.hpp"

#include <map>
#include <mutex>

namespace p4::adaptateurs {

// ADAPTATEUR DE SORTIE : stockage en mémoire, protégé par un mutex
// (le serveur HTTP traite les requêtes sur plusieurs threads).
class PartieRepositoryEnMemoire final : public application::PartieRepository {
public:
    void sauvegarder(const domaine::Partie& partie) override;
    std::optional<domaine::Partie> trouverParId(const domaine::IdPartie& id) const override;
    std::size_t nombreDeParties() const;

private:
    mutable std::mutex verrou_;
    std::map<domaine::IdPartie, domaine::Partie> parties_;
};

} // namespace p4::adaptateurs
