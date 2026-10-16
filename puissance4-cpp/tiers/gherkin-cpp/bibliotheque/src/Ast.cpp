// Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:48:43
#include "gherkin/Ast.hpp"

#include <stdexcept>

namespace gherkin {

const std::vector<std::string>& Table::entetes() const {
    static const std::vector<std::string> aucune;
    return lignes_.empty() ? aucune : lignes_.front();
}

std::vector<std::vector<std::string>> Table::donnees() const {
    if (lignes_.size() < 2) {
        return {};
    }
    return {lignes_.begin() + 1, lignes_.end()};
}

std::vector<std::map<std::string, std::string>> Table::enDictionnaires() const {
    std::vector<std::map<std::string, std::string>> resultat;
    const auto& noms = entetes();
    for (const auto& ligne : donnees()) {
        std::map<std::string, std::string> d;
        for (std::size_t i = 0; i < noms.size() && i < ligne.size(); ++i) {
            d[noms[i]] = ligne[i];
        }
        resultat.push_back(std::move(d));
    }
    return resultat;
}

std::vector<std::string> Table::colonne(const std::string& entete) const {
    const auto& noms = entetes();
    for (std::size_t i = 0; i < noms.size(); ++i) {
        if (noms[i] == entete) {
            std::vector<std::string> valeurs;
            for (const auto& ligne : donnees()) {
                valeurs.push_back(ligne.at(i));
            }
            return valeurs;
        }
    }
    throw std::out_of_range("Colonne absente de la table : " + entete);
}

namespace {

void remplacerTout(std::string& texte, const std::string& motif, const std::string& valeur) {
    for (std::size_t pos = texte.find(motif); pos != std::string::npos; pos = texte.find(motif, pos + valeur.size())) {
        texte.replace(pos, motif.size(), valeur);
    }
}

// Remplace les <paramètres> d'un plan du scénario par les valeurs d'une ligne d'exemples.
Etape substituer(Etape etape, const std::vector<std::string>& noms, const std::vector<std::string>& valeurs) {
    for (std::size_t i = 0; i < noms.size(); ++i) {
        const std::string motif = "<" + noms[i] + ">";
        remplacerTout(etape.texte, motif, valeurs[i]);
        if (etape.docString) {
            remplacerTout(*etape.docString, motif, valeurs[i]);
        }
        if (etape.table) {
            for (auto& ligne : etape.table->lignes()) {
                for (auto& cellule : ligne) {
                    remplacerTout(cellule, motif, valeurs[i]);
                }
            }
        }
    }
    return etape;
}

} // namespace

std::vector<ScenarioConcret> deplier(const Fonctionnalite& f) {
    std::vector<ScenarioConcret> resultat;
    for (const Scenario& sc : f.scenarios) {
        std::vector<std::string> tags = f.tags;
        tags.insert(tags.end(), sc.tags.begin(), sc.tags.end());
        if (!sc.plan) {
            ScenarioConcret c{f.fichier, f.nom, sc.nom, tags, f.contexte, sc.ligne};
            c.etapes.insert(c.etapes.end(), sc.etapes.begin(), sc.etapes.end());
            resultat.push_back(std::move(c));
            continue;
        }
        int numero = 0;
        for (const Exemples& ex : sc.exemples) {
            const auto& noms = ex.table.entetes();
            for (const auto& valeurs : ex.table.donnees()) {
                ++numero;
                std::string detail;
                for (std::size_t i = 0; i < noms.size(); ++i) {
                    detail += (i ? ", " : "") + noms[i] + "=" + valeurs[i];
                }
                ScenarioConcret c{f.fichier, f.nom, sc.nom + " (exemple " + std::to_string(numero) + " : " + detail + ")",
                                  tags, f.contexte, ex.ligne};
                c.tags.insert(c.tags.end(), ex.tags.begin(), ex.tags.end());
                for (const Etape& e : sc.etapes) {
                    c.etapes.push_back(substituer(e, noms, valeurs));
                }
                resultat.push_back(std::move(c));
            }
        }
    }
    return resultat;
}

} // namespace gherkin
