// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#pragma once

#include "domaine/Partie.hpp"

namespace p4::application {

// PORT DE SORTIE : fabrique d'identifiants de partie.
// L'isoler derrière une interface rend les tests déterministes.
class GenerateurIdentifiant {
public:
    virtual ~GenerateurIdentifiant() = default;

    virtual domaine::IdPartie suivant() = 0;
};

} // namespace p4::application
