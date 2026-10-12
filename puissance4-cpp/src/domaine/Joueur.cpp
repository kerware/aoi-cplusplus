// Dernière génération/modification faite par l'IA Claude le 09/10/2026 16:47:56
#include "domaine/Joueur.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>

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
// std::count_if exprime directement l'intention.
std::size_t nombreDeCaracteres(const std::string& texteUtf8) {
    return static_cast<std::size_t>(std::count_if(texteUtf8.begin(), texteUtf8.end(), [](char octet) {
        // on ne compte pas les octets de continuation (10xxxxxx)
        return (static_cast<unsigned char>(octet) & 0xC0) != 0x80;
    }));
}

} // namespace

Joueur::Joueur(const std::string& nom, Jeton couleur)
    : nom_(sansEspacesDeBord(nom)), couleur_(couleur) {
    if (nom_.empty()) {
        throw std::invalid_argument("Le nom du joueur est obligatoire");
    }
    if (nombreDeCaracteres(nom_) > LONGUEUR_MAX_NOM) {
        throw std::invalid_argument("Le nom du joueur ne doit pas dépasser " +
                                    std::to_string(LONGUEUR_MAX_NOM) + " caractères");
    }
    if (couleur_ == Jeton::Vide) {
        throw std::invalid_argument("Un joueur doit avoir la couleur ROUGE ou JAUNE");
    }
}

} // namespace p4::domaine
