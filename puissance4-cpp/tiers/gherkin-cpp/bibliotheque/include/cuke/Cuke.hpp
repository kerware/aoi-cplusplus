// Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:48:43
// ---------------------------------------------------------------------------
// cuke : exécution des scénarios Gherkin en C++, chaque scénario devenant un test GoogleTest.
//
//   SOIT("une calculatrice neuve") { ctx.obtenir<Monde>().calculatrice = Calculatrice{}; }
//   QUAND("j'additionne {int} et {int}") {
//       auto& m = ctx.obtenir<Monde>();
//       m.resultat = m.calculatrice.additionner(arg.entier(0), arg.entier(1));
//   }
//   ALORS("le résultat est {int}") { EXPECT_EQ(ctx.obtenir<Monde>().resultat, arg.entier(0)); }
//
// Dans une définition d'étape : « ctx » (cuke::Contexte, état du scénario) et « arg » (cuke::Arguments).
// Macros : SOIT, QUAND, ALORS, ETAPE (et GIVEN, WHEN, THEN, STEP) — le mot-clé n'influe pas sur la
// correspondance, comme dans Cucumber.
// ---------------------------------------------------------------------------
#pragma once

#include "gherkin/Ast.hpp"
#include "gherkin/Expressions.hpp"

#include <deque>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace cuke {

// État d'un scénario : un objet de chaque type, créé au premier appel, détruit à la fin du scénario.
class Contexte {
public:
    template <class T>
    T& obtenir() {
        auto& objet = objets_[std::type_index(typeid(T))];
        if (!objet) {
            objet = std::make_shared<T>();
        }
        return *static_cast<T*>(objet.get());
    }

private:
    std::unordered_map<std::type_index, std::shared_ptr<void>> objets_;
};

// Paramètres d'une étape : valeurs capturées par l'expression, table de données, doc string.
class Arguments {
public:
    Arguments(std::vector<std::string> valeurs, const gherkin::Etape& etape)
        : valeurs_(std::move(valeurs)), etape_(etape) {}

    std::size_t taille() const { return valeurs_.size(); }
    int entier(std::size_t i) const;
    double reel(std::size_t i) const;            // accepte 3.5 et 3,5
    const std::string& texte(std::size_t i) const;
    const gherkin::Table& table() const;         // lève std::logic_error si l'étape n'a pas de table
    const std::string& docString() const;        // lève std::logic_error si l'étape n'a pas de doc string
    const gherkin::Etape& etape() const { return etape_; }

private:
    std::vector<std::string> valeurs_;
    const gherkin::Etape& etape_;
};

using FonctionEtape = void (*)(Contexte&, const Arguments&);

struct DefinitionEtape {
    gherkin::Expression expression;
    FonctionEtape fonction;
    std::string fichier;
    int ligne;
};

struct Correspondance {
    const DefinitionEtape* definition;
    std::vector<std::string> valeurs;
};

// Registre global des définitions d'étapes, alimenté par les macros à l'initialisation du programme.
class Registre {
public:
    static Registre& instance();

    bool ajouter(const char* expression, FonctionEtape fonction, const char* fichier, int ligne);
    std::vector<Correspondance> trouver(const std::string& texte) const;
    const std::deque<DefinitionEtape>& definitions() const { return definitions_; }
    const std::vector<std::string>& erreurs() const { return erreurs_; }

private:
    std::deque<DefinitionEtape> definitions_;   // deque : adresses stables
    std::vector<std::string> erreurs_;
};

// Étape écrite mais pas encore implémentée : le scénario est « ignoré » (SKIPPED), pas en échec.
class EtapeEnAttente : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

[[noreturn]] void enAttente(const std::string& raison = "étape à implémenter");

// Point d'entrée : analyse les .feature, enregistre un test GoogleTest par scénario et les exécute.
//   <exécutable> [fichiers ou dossiers .feature] [--tags "<expression>"] [--extraits] [options GoogleTest]
int executer(int argc, char** argv, const std::vector<std::string>& cheminsParDefaut);

} // namespace cuke

#define CUKE_CONCAT_(a, b) a##b
#define CUKE_CONCAT(a, b) CUKE_CONCAT_(a, b)
#define CUKE_ETAPE_(expression, fonction)                                                                   \
    static void fonction(cuke::Contexte&, const cuke::Arguments&);                                         \
    [[maybe_unused]] static const bool CUKE_CONCAT(fonction, _enregistree) =                               \
        cuke::Registre::instance().ajouter(expression, &fonction, __FILE__, __LINE__);                      \
    static void fonction([[maybe_unused]] cuke::Contexte& ctx, [[maybe_unused]] const cuke::Arguments& arg)

#define ETAPE(expression) CUKE_ETAPE_(expression, CUKE_CONCAT(cuke_etape_, __LINE__))
#define SOIT(expression) ETAPE(expression)
#define QUAND(expression) ETAPE(expression)
#define ALORS(expression) ETAPE(expression)
#define STEP(expression) ETAPE(expression)
#define GIVEN(expression) ETAPE(expression)
#define WHEN(expression) ETAPE(expression)
#define THEN(expression) ETAPE(expression)
