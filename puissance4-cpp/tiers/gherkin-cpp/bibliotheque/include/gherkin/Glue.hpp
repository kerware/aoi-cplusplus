// Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:48:43
// ---------------------------------------------------------------------------
// Génération du code glue : texte d'une étape -> expression -> définition d'étape C++ à compléter.
//   « Quand j'additionne 2 et 3 »           -> QUAND("j'additionne {int} et {int}")
//   « Alors l'erreur "Division" est signalée » -> ALORS("l'erreur {string} est signalée")
//   « Quand je multiplie <a> par <b> » (plan) -> type choisi d'après les valeurs des Exemples
// ---------------------------------------------------------------------------
#pragma once

#include "gherkin/Ast.hpp"
#include "gherkin/Expressions.hpp"

#include <map>
#include <string>
#include <vector>

namespace gherkin {

struct Extrait {
    std::string expression;                   // expression d'étape (syntaxe Cucumber Expressions)
    std::vector<TypeParametre> parametres;
    std::vector<std::string> noms;            // nom de variable proposé pour chaque paramètre
    TypeEtape type = TypeEtape::Contexte;
    std::string exemple;                      // l'étape d'origine : « Quand j'additionne 2 et 3 »
    std::string origine;                      // « features/calculatrice.feature:12 »
    bool table = false;
    bool docString = false;
};

// Colonnes des Exemples d'un plan du scénario : nom -> valeurs (pour typer les <paramètres>).
using ColonnesExemples = std::map<std::string, std::vector<std::string>>;

Extrait extraitPour(const Etape& etape, const std::string& origine, const ColonnesExemples& colonnes = {});

// Extraits uniques (par expression), dans l'ordre d'apparition, pour toutes les étapes des fonctionnalités.
std::vector<Extrait> extraits(const std::vector<Fonctionnalite>& fonctionnalites);

// Code C++ d'une définition d'étape à compléter.
std::string codeEtape(const Extrait& extrait);

// Fichier .cpp complet de code glue.
std::string fichierGlue(const std::vector<Extrait>& extraits, const std::vector<std::string>& sources);

// Nom utilisable comme identifiant (C++, GoogleTest) : accents retirés, autres caractères -> '_'.
std::string identifiant(const std::string& texte);

} // namespace gherkin
