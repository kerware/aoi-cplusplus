// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#include "adaptateurs/sortie/memoire/PartieRepositoryEnMemoire.hpp"

namespace p4::adaptateurs {

void PartieRepositoryEnMemoire::sauvegarder(const domaine::Partie& partie) {
    std::lock_guard verrou(verrou_);
    parties_.insert_or_assign(partie.id(), partie);
}

std::optional<domaine::Partie> PartieRepositoryEnMemoire::trouverParId(const domaine::IdPartie& id) const {
    std::lock_guard verrou(verrou_);
    const auto trouvee = parties_.find(id);
    if (trouvee == parties_.end()) {
        return std::nullopt;
    }
    return trouvee->second;
}

std::size_t PartieRepositoryEnMemoire::nombreDeParties() const {
    std::lock_guard verrou(verrou_);
    return parties_.size();
}

} // namespace p4::adaptateurs
