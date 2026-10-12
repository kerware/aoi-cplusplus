// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#include "domaine/Partie.hpp"

#include <stdexcept>
#include <utility>

namespace p4::domaine {

std::string_view versTexte(StatutPartie statut) {
    switch (statut) {
        case StatutPartie::EnCours: break;
    }
    return "EN_COURS";
}

Partie::Partie(IdPartie id, const std::string& nomJoueur1, const std::string& nomJoueur2)
    : id_(std::move(id)),
      joueur1_(nomJoueur1, Jeton::Rouge),
      joueur2_(nomJoueur2, Jeton::Jaune) {
    if (id_.empty()) {
        throw std::invalid_argument("L'identifiant de partie est obligatoire");
    }
}

} // namespace p4::domaine
