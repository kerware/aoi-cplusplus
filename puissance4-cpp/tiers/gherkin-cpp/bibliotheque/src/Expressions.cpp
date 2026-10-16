// Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:48:43
#include "gherkin/Expressions.hpp"

#include <stdexcept>
#include <string_view>

namespace gherkin {

const char* nomType(TypeParametre type) {
    switch (type) {
    case TypeParametre::Entier:
        return "{int}";
    case TypeParametre::Reel:
        return "{float}";
    case TypeParametre::Chaine:
        return "{string}";
    case TypeParametre::Mot:
        return "{word}";
    case TypeParametre::Quelconque:
        return "{}";
    }
    return "{}";
}

namespace {

struct Definition {
    std::string_view nom;
    TypeParametre type;
    std::string_view regex;   // chaque définition capture exactement UN groupe
};

// {string} : entre guillemets droits seulement (l'apostrophe est trop fréquente en français).
constexpr Definition DEFINITIONS[] = {
    {"int", TypeParametre::Entier, R"((-?\d+))"},
    {"string", TypeParametre::Chaine, R"re("([^"]*)")re"},
    {"float", TypeParametre::Reel, R"((-?\d+(?:[.,]\d+)?|-?[.,]\d+))"},
    {"word", TypeParametre::Mot, R"(([^\s]+))"},
    {"", TypeParametre::Quelconque, R"((.*))"},
};

void ajouterLitteral(std::string& regex, char c) {
    static const std::string_view speciaux = R"(.^$|()[]{}*+?\/)";
    if (speciaux.find(c) != std::string_view::npos) {
        regex += '\\';
    }
    regex += c;
}

} // namespace

Expression::Expression(std::string source) : source_(std::move(source)) {
    std::string regex = "^";
    for (std::size_t i = 0; i < source_.size(); ++i) {
        const char c = source_[i];
        if (c == '\\' && i + 1 < source_.size()) {
            ajouterLitteral(regex, source_[++i]);
        } else if (c == '{') {
            const auto fin = source_.find('}', i);
            if (fin == std::string::npos) {
                throw std::invalid_argument("accolade non fermée dans l'expression : " + source_);
            }
            const std::string_view nom = std::string_view(source_).substr(i + 1, fin - i - 1);
            bool connu = false;
            for (const auto& def : DEFINITIONS) {
                if (def.nom == nom) {
                    regex += def.regex;
                    parametres_.push_back(def.type);
                    connu = true;
                }
            }
            if (!connu) {
                throw std::invalid_argument("type de paramètre inconnu {" + std::string(nom) + "} dans : " + source_);
            }
            i = fin;
        } else {
            ajouterLitteral(regex, c);
        }
    }
    regex_ = std::regex(regex + "$");
}

std::optional<std::vector<std::string>> Expression::correspondre(const std::string& texte) const {
    std::smatch m;
    if (!std::regex_match(texte, m, regex_)) {
        return std::nullopt;
    }
    std::vector<std::string> valeurs;
    for (std::size_t groupe = 1; groupe < m.size(); ++groupe) {
        valeurs.push_back(m[groupe].str());
    }
    return valeurs;
}

} // namespace gherkin
