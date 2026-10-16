// Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:48:43
// Exécution : un test GoogleTest par scénario concret, étapes jouées dans l'ordre.
//   - étape non définie ou ambiguë : échec, avec le code glue à ajouter ;
//   - cuke::enAttente() : scénario ignoré (SKIPPED) ;
//   - échec d'une vérification (EXPECT_/ASSERT_) ou exception : échec, étapes suivantes sautées.
#include "cuke/Cuke.hpp"

#include "gherkin/Analyseur.hpp"
#include "gherkin/Glue.hpp"
#include "gherkin/Tags.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <set>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

namespace cuke {

// --- Arguments -------------------------------------------------------------------------
int Arguments::entier(std::size_t i) const { return std::stoi(texte(i)); }

double Arguments::reel(std::size_t i) const {
    std::string v = texte(i);
    std::replace(v.begin(), v.end(), ',', '.');
    return std::stod(v);
}

const std::string& Arguments::texte(std::size_t i) const {
    if (i >= valeurs_.size()) {
        throw std::out_of_range("l'étape « " + etape_.texte + " » n'a pas de paramètre n° " + std::to_string(i));
    }
    return valeurs_[i];
}

const gherkin::Table& Arguments::table() const {
    if (!etape_.table) {
        throw std::logic_error("l'étape « " + etape_.texte + " » n'a pas de table de données");
    }
    return *etape_.table;
}

const std::string& Arguments::docString() const {
    if (!etape_.docString) {
        throw std::logic_error("l'étape « " + etape_.texte + " » n'a pas de doc string");
    }
    return *etape_.docString;
}

// --- Registre ------------------------------------------------------------------------------
Registre& Registre::instance() {
    static Registre registre;
    return registre;
}

bool Registre::ajouter(const char* expression, FonctionEtape fonction, const char* fichier, int ligne) {
    try {
        definitions_.push_back(DefinitionEtape{gherkin::Expression(expression), fonction, fichier, ligne});
    } catch (const std::exception& e) {
        // Pas d'exception pendant l'initialisation statique : l'erreur est signalée au démarrage.
        erreurs_.push_back(std::string(fichier) + "(" + std::to_string(ligne) + ") : " + e.what());
    }
    return true;
}

std::vector<Correspondance> Registre::trouver(const std::string& texte) const {
    std::vector<Correspondance> resultat;
    for (const auto& def : definitions_) {
        if (auto valeurs = def.expression.correspondre(texte)) {
            resultat.push_back({&def, std::move(*valeurs)});
        }
    }
    return resultat;
}

void enAttente(const std::string& raison) { throw EtapeEnAttente(raison); }

// --- Exécution d'un scénario ---------------------------------------------------------------
namespace {

void afficher(const gherkin::Etape& e, const char* statut) {
    std::cout << "      " << e.motCle << e.texte << "  [" << statut << "]\n";
}

class TestScenario : public ::testing::Test {
public:
    explicit TestScenario(gherkin::ScenarioConcret scenario) : scenario_(std::move(scenario)) {}

    void TestBody() override {
        std::cout << "    Scénario : " << scenario_.nom << "  (" << scenario_.fichier << ":" << scenario_.ligne << ")\n";
        Contexte ctx;
        bool sauter = false;
        for (const gherkin::Etape& e : scenario_.etapes) {
            if (sauter) {
                afficher(e, "sautée");
                continue;
            }
            const auto correspondances = Registre::instance().trouver(e.texte);
            if (correspondances.empty()) {
                afficher(e, "NON DÉFINIE");
                ADD_FAILURE_AT(scenario_.fichier.c_str(), e.ligne)
                    << "Étape non définie : " << e.motCle << e.texte << "\nCode glue à ajouter :\n"
                    << gherkin::codeEtape(gherkin::extraitPour(e, scenario_.fichier + ":" + std::to_string(e.ligne)));
                sauter = true;
                continue;
            }
            if (correspondances.size() > 1) {
                afficher(e, "AMBIGUË");
                std::string liste;
                for (const auto& c : correspondances) {
                    liste += "\n  " + c.definition->fichier + "(" + std::to_string(c.definition->ligne) + ") : \"" +
                             c.definition->expression.source() + "\"";
                }
                ADD_FAILURE_AT(scenario_.fichier.c_str(), e.ligne)
                    << "Étape ambiguë : " << e.texte << ", plusieurs définitions correspondent :" << liste;
                sauter = true;
                continue;
            }
            try {
                correspondances.front().definition->fonction(ctx, Arguments(correspondances.front().valeurs, e));
            } catch (const EtapeEnAttente& attente) {
                afficher(e, "en attente");
                GTEST_SKIP() << "Étape en attente (" << scenario_.fichier << ":" << e.ligne << ") : " << e.motCle << e.texte
                             << " — " << attente.what();
            } catch (const std::exception& erreur) {
                ADD_FAILURE_AT(scenario_.fichier.c_str(), e.ligne) << "Exception dans l'étape « " << e.texte << " » : "
                                                                   << erreur.what();
            }
            if (HasFailure()) {
                const std::string statut = "ÉCHEC — " + scenario_.fichier + ":" + std::to_string(e.ligne);
                afficher(e, statut.c_str());
                sauter = true;
            } else {
                afficher(e, "ok");
            }
        }
    }

private:
    gherkin::ScenarioConcret scenario_;
};

void enregistrer(const std::vector<gherkin::ScenarioConcret>& scenarios) {
    std::set<std::string> dejaVus;
    for (const auto& sc : scenarios) {
        const std::string suite = gherkin::identifiant(sc.fonctionnalite);
        std::string nom = gherkin::identifiant(sc.nom);
        for (int i = 2; !dejaVus.insert(suite + "." + nom).second; ++i) {
            nom = gherkin::identifiant(sc.nom) + "_" + std::to_string(i);
        }
        ::testing::RegisterTest(suite.c_str(), nom.c_str(), nullptr, nullptr, sc.fichier.c_str(), sc.ligne,
                                [sc]() -> ::testing::Test* { return new TestScenario(sc); });
    }
}

void aide() {
    std::cout << "Usage : <exécutable> [fichiers ou dossiers .feature] [--tags \"<expression>\"] [--extraits]\n"
                 "                     [options GoogleTest : --gtest_filter=…, --gtest_output=xml:rapport.xml…]\n"
                 "  --tags      : ne jouer que les scénarios dont les tags correspondent (ex. \"@rapide and not @lent\")\n"
                 "  --extraits  : afficher le code glue des étapes non encore définies, sans rien exécuter\n";
}

} // namespace

int executer(int argc, char** argv, const std::vector<std::string>& cheminsParDefaut) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);   // accents des fichiers .feature dans la console
#endif
    ::testing::InitGoogleTest(&argc, argv);   // retire les options --gtest_*
    std::vector<std::filesystem::path> chemins;
    std::string tags;
    bool modeExtraits = false;
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        if (a == "--tags" && i + 1 < argc) {
            tags = argv[++i];
        } else if (a.rfind("--tags=", 0) == 0) {
            tags = a.substr(7);
        } else if (a == "--extraits") {
            modeExtraits = true;
        } else if (a == "--aide" || a == "--help") {
            aide();
            return 0;
        } else {
            chemins.emplace_back(a);
        }
    }
    if (chemins.empty()) {
        chemins.assign(cheminsParDefaut.begin(), cheminsParDefaut.end());
    }
    if (!Registre::instance().erreurs().empty()) {
        for (const auto& e : Registre::instance().erreurs()) {
            std::cerr << "Définition d'étape invalide : " << e << "\n";
        }
        return 2;
    }
    std::vector<gherkin::Fonctionnalite> fonctionnalites;
    std::vector<gherkin::ScenarioConcret> scenarios;
    try {
        const gherkin::FiltreTags filtre(tags);
        for (const auto& fichier : gherkin::trouverFichiers(chemins)) {
            fonctionnalites.push_back(gherkin::lireFichier(fichier));
            for (auto& sc : gherkin::deplier(fonctionnalites.back())) {
                if (filtre.accepte(sc.tags)) {
                    scenarios.push_back(std::move(sc));
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Erreur : " << e.what() << "\n";
        return 2;
    }

    if (modeExtraits) {
        // Code glue des étapes qu'aucune définition ne reconnaît (à copier dans un fichier d'étapes).
        // Les extraits viennent des étapes écrites (plans : <paramètres> typés d'après les Exemples).
        std::vector<gherkin::Extrait> nonDefinis;
        std::set<std::string> vus;
        for (const auto& f : fonctionnalites) {
            const auto candidats = gherkin::extraits({f});
            for (const auto& sc : gherkin::deplier(f)) {
                for (const auto& e : sc.etapes) {
                    if (Registre::instance().trouver(e.texte).empty()) {
                        for (const auto& x : candidats) {
                            if (gherkin::Expression(x.expression).correspondre(e.texte) && vus.insert(x.expression).second) {
                                nonDefinis.push_back(x);
                            }
                        }
                    }
                }
            }
        }
        if (nonDefinis.empty()) {
            std::cout << "Toutes les étapes sont définies.\n";
            return 0;
        }
        std::cout << "// " << nonDefinis.size() << " étape(s) non définie(s) : code glue à ajouter\n";
        for (const auto& x : nonDefinis) {
            std::cout << "\n" << gherkin::codeEtape(x);
        }
        return 0;
    }

    enregistrer(scenarios);
    return RUN_ALL_TESTS();
}

} // namespace cuke
