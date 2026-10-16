// Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:48:43
// Analyseur (parser) Gherkin : texte .feature -> gherkin::Fonctionnalite
// Langues : anglais (par défaut) et français (« # language: fr » en première ligne).
#pragma once

#include "gherkin/Ast.hpp"

#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace gherkin {

class ErreurGherkin : public std::runtime_error {
public:
    ErreurGherkin(const std::string& fichier, int ligne, const std::string& message);
    const std::string& fichier() const { return fichier_; }
    int ligne() const { return ligne_; }

private:
    std::string fichier_;
    int ligne_;
};

// Analyse le texte d'un fichier .feature (lève ErreurGherkin avec le numéro de ligne).
Fonctionnalite analyser(std::string_view texte, const std::string& fichier = "<texte>");

// Lit et analyse un fichier .feature.
Fonctionnalite lireFichier(const std::filesystem::path& chemin);

// Liste les fichiers .feature d'une liste de fichiers et de dossiers (récursif, ordre alphabétique).
std::vector<std::filesystem::path> trouverFichiers(const std::vector<std::filesystem::path>& chemins);

} // namespace gherkin
