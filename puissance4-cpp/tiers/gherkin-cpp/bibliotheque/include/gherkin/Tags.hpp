// Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:48:43
// Expressions de tags (syntaxe Cucumber) : "@rapide", "not @lent", "@a and (@b or @c)".
#pragma once

#include <memory>
#include <string>
#include <vector>

namespace gherkin {

class FiltreTags {
public:
    FiltreTags();                                     // accepte tout
    explicit FiltreTags(const std::string& expression); // lève std::invalid_argument si invalide
    ~FiltreTags();
    FiltreTags(FiltreTags&&) noexcept;
    FiltreTags& operator=(FiltreTags&&) noexcept;

    bool accepte(const std::vector<std::string>& tags) const;

    struct Noeud;

private:
    std::unique_ptr<Noeud> racine_;
};

} // namespace gherkin
