// Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:48:43
#include "gherkin/Glue.hpp"

#include <algorithm>
#include <cctype>
#include <regex>
#include <set>
#include <sstream>

namespace gherkin {

namespace {

bool alnum(char c) { return std::isalnum(static_cast<unsigned char>(c)) || c == '_'; }

bool estEntier(const std::string& v) { return std::regex_match(v, std::regex(R"(-?\d+)")); }
bool estReel(const std::string& v) { return std::regex_match(v, std::regex(R"(-?\d*[.,]\d+)")); }

// Type d'un <paramètre> de plan d'après toutes ses valeurs d'exemples.
TypeParametre typeColonne(const std::vector<std::string>& valeurs) {
    if (!valeurs.empty() && std::all_of(valeurs.begin(), valeurs.end(), estEntier)) {
        return TypeParametre::Entier;
    }
    if (!valeurs.empty() && std::all_of(valeurs.begin(), valeurs.end(),
                                        [](const std::string& v) { return estEntier(v) || estReel(v); })) {
        return TypeParametre::Reel;
    }
    if (std::none_of(valeurs.begin(), valeurs.end(),
                     [](const std::string& v) { return v.empty() || v.find(' ') != std::string::npos; })) {
        return TypeParametre::Mot;
    }
    return TypeParametre::Quelconque;
}

const char* prefixeNom(TypeParametre type) {
    switch (type) {
    case TypeParametre::Entier:
        return "entier";
    case TypeParametre::Reel:
        return "reel";
    case TypeParametre::Chaine:
        return "texte";
    case TypeParametre::Mot:
        return "mot";
    case TypeParametre::Quelconque:
        return "valeur";
    }
    return "valeur";
}

const char* macro(TypeEtape type) {
    switch (type) {
    case TypeEtape::Contexte:
        return "SOIT";
    case TypeEtape::Action:
        return "QUAND";
    case TypeEtape::Resultat:
        return "ALORS";
    }
    return "ETAPE";
}

std::string litteralCpp(const std::string& s) {
    std::string r = "\"";
    for (const char c : s) {
        if (c == '"' || c == '\\') {
            r += '\\';
        }
        r += c;
    }
    return r + "\"";
}

} // namespace

std::string identifiant(const std::string& texte) {
    // Translittération des lettres accentuées du français (UTF-8 sur deux octets).
    static const std::pair<const char*, const char*> accents[] = {
        {"à", "a"}, {"â", "a"}, {"ä", "a"}, {"ç", "c"}, {"é", "e"}, {"è", "e"}, {"ê", "e"}, {"ë", "e"},
        {"î", "i"}, {"ï", "i"}, {"ô", "o"}, {"ö", "o"}, {"ù", "u"}, {"û", "u"}, {"ü", "u"}, {"ÿ", "y"},
        {"À", "A"}, {"Â", "A"}, {"Ç", "C"}, {"É", "E"}, {"È", "E"}, {"Ê", "E"}, {"Î", "I"}, {"Ô", "O"},
        {"Ù", "U"}, {"Û", "U"}, {"œ", "oe"}, {"Œ", "OE"}, {"æ", "ae"}};
    std::string s = texte;
    for (const auto& [accent, simple] : accents) {
        for (std::size_t pos = s.find(accent); pos != std::string::npos; pos = s.find(accent, pos)) {
            s.replace(pos, std::char_traits<char>::length(accent), simple);
        }
    }
    std::string r;
    for (const char c : s) {
        if (alnum(c) && static_cast<unsigned char>(c) < 128) {
            r += c;
        } else if (!r.empty() && r.back() != '_') {
            r += '_';
        }
    }
    while (!r.empty() && r.back() == '_') {
        r.pop_back();
    }
    if (r.empty() || std::isdigit(static_cast<unsigned char>(r.front()))) {
        r = "p" + r;
    }
    return r;
}

Extrait extraitPour(const Etape& etape, const std::string& origine, const ColonnesExemples& colonnes) {
    Extrait x;
    x.type = etape.type;
    x.exemple = etape.motCle + etape.texte;
    x.origine = origine;
    x.table = etape.table.has_value();
    x.docString = etape.docString.has_value();
    std::map<std::string, int> compteurs;
    auto nouveauNom = [&](TypeParametre type, std::string nom = {}) {
        if (nom.empty()) {
            nom = prefixeNom(type) + std::to_string(++compteurs[prefixeNom(type)]);
        }
        while (std::find(x.noms.begin(), x.noms.end(), nom) != x.noms.end()) {
            nom += "_";
        }
        x.parametres.push_back(type);
        x.noms.push_back(nom);
    };
    const std::string& t = etape.texte;
    static const std::regex nombre(R"(^-?\d+(?:[.,]\d+)?)");
    for (std::size_t i = 0; i < t.size();) {
        const char c = t[i];
        // "texte entre guillemets" -> {string}
        if (c == '"') {
            const auto fin = t.find('"', i + 1);
            if (fin != std::string::npos) {
                x.expression += "{string}";
                nouveauNom(TypeParametre::Chaine);
                i = fin + 1;
                continue;
            }
        }
        // <paramètre> d'un plan du scénario -> type d'après les exemples
        if (c == '<') {
            const auto fin = t.find('>', i + 1);
            if (fin != std::string::npos) {
                const std::string nom = t.substr(i + 1, fin - i - 1);
                if (auto col = colonnes.find(nom); col != colonnes.end()) {
                    const TypeParametre type = typeColonne(col->second);
                    x.expression += nomType(type);
                    nouveauNom(type, identifiant(nom));
                    i = fin + 1;
                    continue;
                }
            }
        }
        // nombre isolé -> {int} ou {float}
        const bool debutDeMot = i == 0 || !alnum(t[i - 1]);
        std::smatch m;
        const std::string reste = t.substr(i);
        if (debutDeMot && (std::isdigit(static_cast<unsigned char>(c)) || c == '-') &&
            std::regex_search(reste, m, nombre)) {
            const std::size_t fin = i + static_cast<std::size_t>(m.length(0));
            if (fin == t.size() || !alnum(t[fin])) {
                const bool reel = m.str(0).find_first_of(".,") != std::string::npos;
                x.expression += reel ? "{float}" : "{int}";
                nouveauNom(reel ? TypeParametre::Reel : TypeParametre::Entier);
                i = fin;
                continue;
            }
        }
        // caractères spéciaux des Cucumber Expressions : échappés
        if (c == '{' || c == '}' || c == '(' || c == ')' || c == '/' || c == '\\') {
            x.expression += '\\';
        }
        x.expression += c;
        ++i;
    }
    return x;
}

std::vector<Extrait> extraits(const std::vector<Fonctionnalite>& fonctionnalites) {
    std::vector<Extrait> resultat;
    std::set<std::string> vues;
    auto ajouter = [&](const Etape& e, const std::string& fichier, const ColonnesExemples& colonnes) {
        Extrait x = extraitPour(e, fichier + ":" + std::to_string(e.ligne), colonnes);
        if (vues.insert(x.expression).second) {
            resultat.push_back(std::move(x));
        }
    };
    for (const auto& f : fonctionnalites) {
        for (const auto& e : f.contexte) {
            ajouter(e, f.fichier, {});
        }
        for (const auto& sc : f.scenarios) {
            ColonnesExemples colonnes;
            for (const auto& ex : sc.exemples) {
                const auto& noms = ex.table.entetes();
                for (std::size_t i = 0; i < noms.size(); ++i) {
                    for (const auto& ligne : ex.table.donnees()) {
                        colonnes[noms[i]].push_back(ligne[i]);
                    }
                }
            }
            for (const auto& e : sc.etapes) {
                ajouter(e, f.fichier, colonnes);
            }
        }
    }
    return resultat;
}

std::string codeEtape(const Extrait& x) {
    std::ostringstream c;
    c << "// " << x.origine << " — " << x.exemple << "\n";
    c << macro(x.type) << "(" << litteralCpp(x.expression) << ") {\n";
    for (std::size_t i = 0; i < x.parametres.size(); ++i) {
        switch (x.parametres[i]) {
        case TypeParametre::Entier:
            c << "    [[maybe_unused]] const int " << x.noms[i] << " = arg.entier(" << i << ");\n";
            break;
        case TypeParametre::Reel:
            c << "    [[maybe_unused]] const double " << x.noms[i] << " = arg.reel(" << i << ");\n";
            break;
        default:
            c << "    [[maybe_unused]] const std::string& " << x.noms[i] << " = arg.texte(" << i << ");\n";
            break;
        }
    }
    if (x.table) {
        c << "    [[maybe_unused]] const gherkin::Table& table = arg.table();   // table.enDictionnaires(), table.colonne(\"…\")\n";
    }
    if (x.docString) {
        c << "    [[maybe_unused]] const std::string& docString = arg.docString();\n";
    }
    c << "    cuke::enAttente();   // À FAIRE : appeler le code à tester, puis vérifier (EXPECT_…)\n";
    c << "}\n";
    return c.str();
}

std::string fichierGlue(const std::vector<Extrait>& liste, const std::vector<std::string>& sources) {
    std::ostringstream c;
    c << "// Code glue généré par gherkin-glue à partir de :\n";
    for (const auto& s : sources) {
        c << "//   " << s << "\n";
    }
    c << "// À COMPLÉTER : dans chaque étape, remplacer cuke::enAttente() par l'appel au code à tester\n"
         "// et des vérifications GoogleTest (EXPECT_EQ…). Les paramètres sont dans « arg ».\n"
         "// Données partagées par les étapes d'un même scénario : ctx.obtenir<Monde>().\n"
         "#include \"cuke/Cuke.hpp\"\n\n"
         "#include <gtest/gtest.h>\n\n"
         "#include <string>\n\n"
         "namespace {\n\n"
         "// État d'un scénario : un objet neuf pour chaque scénario.\n"
         "struct Monde {\n"
         "};\n\n"
         "} // namespace\n";
    for (const auto& x : liste) {
        c << "\n" << codeEtape(x);
    }
    return c.str();
}

} // namespace gherkin
