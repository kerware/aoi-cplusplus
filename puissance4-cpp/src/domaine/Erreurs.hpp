// Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:23:27
#pragma once

#include <stdexcept>
#include <string>

namespace p4::domaine {

// Coup refusé car la colonne n'existe pas (EX-08). Traduit en HTTP 400.
class CoupInvalide : public std::invalid_argument {
public:
    explicit CoupInvalide(int colonne)
        : std::invalid_argument("Colonne invalide : doit être un entier entre 0 et 6 (reçu : " +
                                std::to_string(colonne) + ")") {}
};

// Coup refusé car la colonne est déjà remplie (EX-09). Traduit en HTTP 409.
class ColonnePleine : public std::logic_error {
public:
    explicit ColonnePleine(int colonne)
        : std::logic_error("Colonne pleine : " + std::to_string(colonne)) {}
};

// Coup refusé car la partie est terminée (EX-17). Traduit en HTTP 409.
class PartieTerminee : public std::logic_error {
public:
    PartieTerminee() : std::logic_error("Partie terminée : plus aucun coup n'est accepté") {}
};

} // namespace p4::domaine
