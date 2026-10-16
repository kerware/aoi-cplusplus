// Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:48:43
// main() des exécutables de scénarios, compilé dans chaque cible créée par gherkin_ajouter_executable().
// CUKE_FEATURES : dossier des .feature par défaut (fixé par CMake).
#include "cuke/Cuke.hpp"

#ifndef CUKE_FEATURES
#define CUKE_FEATURES "features"
#endif

int main(int argc, char** argv) { return cuke::executer(argc, argv, {CUKE_FEATURES}); }
