// Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:48:43
// ---------------------------------------------------------------------------
// gherkin-glue : lit des fichiers .feature et génère le squelette du code glue C++ (définitions d'étapes).
//   gherkin-glue features                                   # affiche le code glue
//   gherkin-glue features --sortie etapes/etapes_generees.cpp
//   gherkin-glue features --sortie … --forcer               # écrase un fichier existant (code perdu !)
// Un fichier existant n'est jamais écrasé sans --forcer : c'est le code que vous avez complété. Pour les
// étapes ajoutées ensuite, lancez l'exécutable de scénarios avec --extraits.
// ---------------------------------------------------------------------------
#include "gherkin/Analyseur.hpp"
#include "gherkin/Glue.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

int main(int argc, char** argv) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    std::vector<std::filesystem::path> chemins;
    std::filesystem::path sortie;
    bool forcer = false;
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        if (a == "--sortie" && i + 1 < argc) {
            sortie = argv[++i];
        } else if (a == "--forcer") {
            forcer = true;
        } else if (a == "--aide" || a == "--help") {
            std::cout << "Usage : gherkin-glue <fichiers ou dossiers .feature> [--sortie fichier.cpp] [--forcer]\n";
            return 0;
        } else {
            chemins.emplace_back(a);
        }
    }
    if (chemins.empty()) {
        std::cerr << "Usage : gherkin-glue <fichiers ou dossiers .feature> [--sortie fichier.cpp] [--forcer]\n";
        return 2;
    }
    try {
        std::vector<gherkin::Fonctionnalite> fonctionnalites;
        std::vector<std::string> sources;
        for (const auto& fichier : gherkin::trouverFichiers(chemins)) {
            fonctionnalites.push_back(gherkin::lireFichier(fichier));
            sources.push_back(fichier.generic_string());
        }
        const auto extraits = gherkin::extraits(fonctionnalites);
        const std::string code = gherkin::fichierGlue(extraits, sources);
        if (sortie.empty()) {
            std::cout << code;
            return 0;
        }
        if (std::filesystem::exists(sortie) && !forcer) {
            std::cout << "gherkin-glue : " << sortie.generic_string()
                      << " existe déjà, il n'est pas écrasé (c'est votre code).\n"
                         "  Étapes nouvelles : lancez l'exécutable de scénarios avec --extraits.\n";
            return 0;
        }
        if (sortie.has_parent_path()) {
            std::filesystem::create_directories(sortie.parent_path());
        }
        std::ofstream(sortie, std::ios::binary) << code;
        std::cout << "gherkin-glue : " << extraits.size() << " définition(s) d'étape générée(s) dans "
                  << sortie.generic_string() << " (" << fonctionnalites.size() << " fichier(s) .feature)\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "gherkin-glue : " << e.what() << "\n";
        return 1;
    }
}
