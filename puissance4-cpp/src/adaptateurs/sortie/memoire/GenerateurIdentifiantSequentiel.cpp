// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#include "adaptateurs/sortie/memoire/GenerateurIdentifiantSequentiel.hpp"

#include <string>

namespace p4::adaptateurs {

domaine::IdPartie GenerateurIdentifiantSequentiel::suivant() {
    const int numero = ++compteur_;
    std::string chiffres = std::to_string(numero);
    if (chiffres.size() < 4) {
        chiffres.insert(0, 4 - chiffres.size(), '0');
    }
    return "P" + chiffres;
}

} // namespace p4::adaptateurs
