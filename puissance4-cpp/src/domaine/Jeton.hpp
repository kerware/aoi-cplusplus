// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#pragma once

#include <string_view>

namespace p4::domaine {

// Contenu d'une case de la grille / couleur d'un joueur.
enum class Jeton { Vide, Rouge, Jaune };

// "VIDE", "ROUGE", "JAUNE" — utilisé pour les messages et la sérialisation.
std::string_view versTexte(Jeton jeton);

// Symbole d'affichage compact : '.', 'R', 'J'.
char versSymbole(Jeton jeton);

} // namespace p4::domaine
