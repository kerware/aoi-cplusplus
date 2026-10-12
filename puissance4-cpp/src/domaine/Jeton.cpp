// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#include "domaine/Jeton.hpp"

namespace p4::domaine {

std::string_view versTexte(Jeton jeton) {
    switch (jeton) {
        case Jeton::Rouge: return "ROUGE";
        case Jeton::Jaune: return "JAUNE";
        case Jeton::Vide:  break;
    }
    return "VIDE";
}

char versSymbole(Jeton jeton) {
    switch (jeton) {
        case Jeton::Rouge: return 'R';
        case Jeton::Jaune: return 'J';
        case Jeton::Vide:  break;
    }
    return '.';
}

} // namespace p4::domaine
