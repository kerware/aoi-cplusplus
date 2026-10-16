// Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:48:43
// ---------------------------------------------------------------------------
// Arbre syntaxique d'un fichier Gherkin (.feature)
//   Fonctionnalité (Feature) > Contexte (Background) + Scénarios / Plans du scénario (Scenario Outline)
//   Scénario > Étapes (Soit / Quand / Alors / Et / Mais) > table de données ou doc string
// ---------------------------------------------------------------------------
#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace gherkin {

// Table de données : la première ligne contient les en-têtes.
class Table {
public:
    Table() = default;
    explicit Table(std::vector<std::vector<std::string>> lignes) : lignes_(std::move(lignes)) {}

    const std::vector<std::vector<std::string>>& lignes() const { return lignes_; }
    std::vector<std::vector<std::string>>& lignes() { return lignes_; }
    bool vide() const { return lignes_.empty(); }

    // En-têtes (première ligne) et lignes de données (les suivantes).
    const std::vector<std::string>& entetes() const;
    std::vector<std::vector<std::string>> donnees() const;

    // Chaque ligne de données sous forme { en-tête -> valeur }.
    std::vector<std::map<std::string, std::string>> enDictionnaires() const;

    // Valeurs d'une colonne désignée par son en-tête (lève std::out_of_range si absente).
    std::vector<std::string> colonne(const std::string& entete) const;

private:
    std::vector<std::vector<std::string>> lignes_;
};

// Rôle d'une étape. « Et », « Mais » et « * » prennent le rôle de l'étape précédente.
enum class TypeEtape { Contexte, Action, Resultat };

struct Etape {
    std::string motCle;            // tel qu'écrit : "Soit ", "Quand ", "Et ", "Given "…
    std::string texte;             // le texte après le mot-clé
    TypeEtape type = TypeEtape::Contexte;
    std::optional<Table> table;
    std::optional<std::string> docString;
    int ligne = 0;
};

struct Exemples {
    std::string nom;
    std::vector<std::string> tags;
    Table table;
    int ligne = 0;
};

struct Scenario {
    std::string motCle;
    std::string nom;
    std::vector<std::string> tags;
    std::vector<Etape> etapes;
    std::vector<Exemples> exemples;   // non vide pour un plan du scénario
    bool plan = false;
    int ligne = 0;
};

struct Fonctionnalite {
    std::string fichier;
    std::string langue = "en";
    std::string nom;
    std::string description;
    std::vector<std::string> tags;
    std::vector<Etape> contexte;      // étapes du Contexte (Background), jouées avant chaque scénario
    std::vector<Scenario> scenarios;
    int ligne = 0;
};

// Scénario prêt à être exécuté : contexte + étapes, plan du scénario déplié pour une ligne d'exemples.
struct ScenarioConcret {
    std::string fichier;
    std::string fonctionnalite;
    std::string nom;                  // pour un plan : "<nom> (exemple 2 : a=4, b=5)"
    std::vector<std::string> tags;    // tags de la fonctionnalité + du scénario (+ des exemples)
    std::vector<Etape> etapes;
    int ligne = 0;
};

// Déplie une fonctionnalité en scénarios concrets (un par scénario, un par ligne d'exemples).
std::vector<ScenarioConcret> deplier(const Fonctionnalite& fonctionnalite);

} // namespace gherkin
