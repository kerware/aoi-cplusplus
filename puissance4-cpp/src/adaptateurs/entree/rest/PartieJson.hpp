// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#pragma once

#include "domaine/Partie.hpp"

#include <nlohmann/json.hpp>

namespace p4::adaptateurs {

// Traduction domaine -> JSON (le domaine ne connaît pas JSON).
// Exemple :
// {
//   "id": "P0001", "statut": "EN_COURS", "joueurCourant": "Alice",
//   "joueur1": {"nom": "Alice", "couleur": "ROUGE"},
//   "joueur2": {"nom": "Bob",   "couleur": "JAUNE"},
//   "grille": [".......", ".......", ".......", ".......", ".......", "......."]
// }
nlohmann::json versJson(const domaine::Partie& partie);

} // namespace p4::adaptateurs
