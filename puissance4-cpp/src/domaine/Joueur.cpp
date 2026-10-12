// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#include "domaine/Joueur.hpp"

#include <stdexcept>

namespace p4::domaine {

namespace {

std::string sansEspacesDeBord(const std::string& texte) {
    const auto debut = texte.find_first_not_of(" \t\r\n");
    if (debut == std::string::npos) {
        return {};
    }
    const auto fin = texte.find_last_not_of(" \t\r\n");
    return texte.substr(debut, fin - debut + 1);
}

// Nombre de caractères d'une chaîne UTF-8 (et non nombre d'octets) : "Éloïse" = 6.
std::size_t nombreDeCaracteres(const std::string& texteUtf8) {
    std::size_t total = 0;
    for (unsigned char octet : texteUtf8) {
        if ((octet & 0xC0) != 0x80) {  // on ne compte pas les octets de continuation
            ++total;
        }
    }
    return total;
}

} // namespace

Joueur::Joueur(std::string nom, Jeton couleur)
    : nom_(sansEspacesDeBord(nom)), couleur_(couleur) {
    if (nom_.empty()) {
        throw std::invalid_argument("Le nom du joueur est obligatoire");
    }
    if (nombreDeCaracteres(nom_) > LONGUEUR_MAX_NOM) {
        throw std::invalid_argument("Le nom du joueur ne doit pas dépasser 20 caractères");
    }
    if (couleur_ == Jeton::Vide) {
        throw std::invalid_argument("Un joueur doit avoir la couleur ROUGE ou JAUNE");
    }
}

} // namespace p4::domaine
