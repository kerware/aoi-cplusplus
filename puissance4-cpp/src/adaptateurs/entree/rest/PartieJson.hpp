// Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:23:27
#pragma once

#include "domaine/Partie.hpp"

#include <nlohmann/json.hpp>

namespace p4::adaptateurs {

// Traduction domaine -> JSON (le domaine ne connaît pas JSON).
// Exemple :
// {
//   "id": "P0001", "statut": "EN_COURS", "joueurCourant": "Bob",
//   "joueur1": {"nom": "Alice", "couleur": "ROUGE"},
//   "joueur2": {"nom": "Bob",   "couleur": "JAUNE"},
//   "nombreDeCoups": 1,
//   "dernierCoup": {"colonne": 3, "ligne": 5, "couleur": "ROUGE", "joueur": "Alice"},
//   "grille": [".......", ".......", ".......", ".......", ".......", "...R..."]
// }
nlohmann::json versJson(const domaine::Partie& partie);

} // namespace p4::adaptateurs
