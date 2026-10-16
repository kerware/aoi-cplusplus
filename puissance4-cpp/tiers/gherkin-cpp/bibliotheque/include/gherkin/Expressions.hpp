// Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:48:43
// ---------------------------------------------------------------------------
// Expressions d'étapes, sous-ensemble des « Cucumber Expressions » :
//   "j'additionne {int} et {int}"   {int} {float} {string} {word} {}
//   {string} : texte entre guillemets droits "…" (la valeur est sans les guillemets ; pas d'apostrophes,
//              trop fréquentes en français : « j'additionne »)
//   \{ \} \( \) \/ \\ : caractères littéraux
// Non pris en charge : texte optionnel « (s) », alternatives « a/b », types personnalisés.
// ---------------------------------------------------------------------------
#pragma once

#include <optional>
#include <regex>
#include <string>
#include <vector>

namespace gherkin {

enum class TypeParametre { Entier, Reel, Chaine, Mot, Quelconque };

// "{int}", "{float}"…
const char* nomType(TypeParametre type);

class Expression {
public:
    explicit Expression(std::string source);   // lève std::invalid_argument si l'expression est invalide

    const std::string& source() const { return source_; }
    const std::vector<TypeParametre>& parametres() const { return parametres_; }

    // Valeurs des paramètres si le texte de l'étape correspond (en entier) à l'expression.
    std::optional<std::vector<std::string>> correspondre(const std::string& texte) const;

private:
    std::string source_;
    std::regex regex_;
    std::vector<TypeParametre> parametres_;
};

} // namespace gherkin
