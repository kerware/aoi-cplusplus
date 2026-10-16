// Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:48:43
// Analyse descendante : ou := et ("or" et)* ; et := non ("and" non)* ; non := "not" non | "(" ou ")" | @tag
#include "gherkin/Tags.hpp"

#include <algorithm>
#include <stdexcept>

namespace gherkin {

struct FiltreTags::Noeud {
    enum class Sorte { Tag, Non, Et, Ou } sorte;
    std::string tag;
    std::unique_ptr<Noeud> gauche, droite;

    bool evaluer(const std::vector<std::string>& tags) const {
        switch (sorte) {
        case Sorte::Tag:
            return std::find(tags.begin(), tags.end(), tag) != tags.end();
        case Sorte::Non:
            return !gauche->evaluer(tags);
        case Sorte::Et:
            return gauche->evaluer(tags) && droite->evaluer(tags);
        case Sorte::Ou:
            return gauche->evaluer(tags) || droite->evaluer(tags);
        }
        return false;
    }
};

namespace {

using Noeud = FiltreTags::Noeud;

class Analyse {
public:
    explicit Analyse(const std::string& s) : source_(s) {
        std::string mot;
        for (const char c : s) {
            if (c == '(' || c == ')' || c == ' ') {
                if (!mot.empty()) {
                    jetons_.push_back(mot);
                    mot.clear();
                }
                if (c != ' ') {
                    jetons_.emplace_back(1, c);
                }
            } else {
                mot += c;
            }
        }
        if (!mot.empty()) {
            jetons_.push_back(mot);
        }
    }

    std::unique_ptr<Noeud> executer() {
        auto r = ou();
        if (pos_ != jetons_.size()) {
            erreur();
        }
        return r;
    }

private:
    [[noreturn]] void erreur() const { throw std::invalid_argument("expression de tags invalide : " + source_); }
    bool est(const char* jeton) const { return pos_ < jetons_.size() && jetons_[pos_] == jeton; }

    static std::unique_ptr<Noeud> binaire(Noeud::Sorte sorte, std::unique_ptr<Noeud> g, std::unique_ptr<Noeud> d) {
        auto n = std::make_unique<Noeud>();
        n->sorte = sorte;
        n->gauche = std::move(g);
        n->droite = std::move(d);
        return n;
    }

    std::unique_ptr<Noeud> ou() {
        auto g = et();
        while (est("or")) {
            ++pos_;
            g = binaire(Noeud::Sorte::Ou, std::move(g), et());
        }
        return g;
    }

    std::unique_ptr<Noeud> et() {
        auto g = non();
        while (est("and")) {
            ++pos_;
            g = binaire(Noeud::Sorte::Et, std::move(g), non());
        }
        return g;
    }

    std::unique_ptr<Noeud> non() {
        if (est("not")) {
            ++pos_;
            auto n = std::make_unique<Noeud>();
            n->sorte = Noeud::Sorte::Non;
            n->gauche = non();
            return n;
        }
        if (est("(")) {
            ++pos_;
            auto n = ou();
            if (!est(")")) {
                erreur();
            }
            ++pos_;
            return n;
        }
        if (pos_ < jetons_.size() && jetons_[pos_].size() > 1 && jetons_[pos_][0] == '@') {
            auto n = std::make_unique<Noeud>();
            n->sorte = Noeud::Sorte::Tag;
            n->tag = jetons_[pos_++];
            return n;
        }
        erreur();
    }

    std::string source_;
    std::vector<std::string> jetons_;
    std::size_t pos_ = 0;
};

} // namespace

FiltreTags::FiltreTags() = default;
FiltreTags::~FiltreTags() = default;
FiltreTags::FiltreTags(FiltreTags&&) noexcept = default;
FiltreTags& FiltreTags::operator=(FiltreTags&&) noexcept = default;

FiltreTags::FiltreTags(const std::string& expression) {
    if (expression.find_first_not_of(' ') != std::string::npos) {
        racine_ = Analyse(expression).executer();
    }
}

bool FiltreTags::accepte(const std::vector<std::string>& tags) const {
    return !racine_ || racine_->evaluer(tags);
}

} // namespace gherkin
