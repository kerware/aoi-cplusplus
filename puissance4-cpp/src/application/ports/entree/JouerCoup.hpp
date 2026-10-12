// Dernière génération/modification faite par l'IA Claude le 09/10/2026 15:10:00
#pragma once

#include "domaine/Partie.hpp"

namespace p4::application {

// PORT D'ENTRÉE (itération 2) : jouer un coup dans une partie existante.
class JouerCoup {
public:
    virtual ~JouerCoup() = default;

    // Retourne la partie à jour (EX-11).
    // Lève PartieInconnue (EX-10), domaine::CoupInvalide (EX-08), domaine::ColonnePleine (EX-09).
    virtual domaine::Partie jouer(const domaine::IdPartie& id, int colonne) = 0;
};

} // namespace p4::application
