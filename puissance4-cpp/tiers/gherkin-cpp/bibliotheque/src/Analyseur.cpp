// Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:48:43
// Analyseur Gherkin ligne à ligne. Sous-ensemble pris en charge : Feature / Fonctionnalité, Background /
// Contexte, Scenario / Scénario, Scenario Outline / Plan du scénario + Examples / Exemples, Rule / Règle
// (regroupement seulement), tags, commentaires, tables de données, doc strings (""" ou ```).
#include "gherkin/Analyseur.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <utility>

namespace gherkin {

ErreurGherkin::ErreurGherkin(const std::string& fichier, int ligne, const std::string& message)
    : std::runtime_error(fichier + ":" + std::to_string(ligne) + " : " + message), fichier_(fichier), ligne_(ligne) {}

namespace {

// Mots-clés d'une langue (extrait de gherkin-languages.json du projet Cucumber).
struct MotsCles {
    std::vector<std::string> fonctionnalite, contexte, scenario, plan, exemples, regle;
    std::vector<std::string> soit, quand, alors, et, mais;
};

const MotsCles& motsCles(const std::string& langue, const std::string& fichier) {
    static const MotsCles en{{"Feature", "Business Need", "Ability"},
                             {"Background"},
                             {"Scenario", "Example"},
                             {"Scenario Outline", "Scenario Template"},
                             {"Examples", "Scenarios"},
                             {"Rule"},
                             {"Given "},
                             {"When "},
                             {"Then "},
                             {"And "},
                             {"But "}};
    static const MotsCles fr{{"Fonctionnalité"},
                             {"Contexte"},
                             {"Scénario", "Exemple"},
                             {"Plan du scénario", "Plan du Scénario"},
                             {"Exemples"},
                             {"Règle"},
                             {"Soit ", "Sachant que ", "Sachant qu'", "Sachant ", "Étant donné que ", "Étant donné qu'",
                              "Étant donné ", "Étant donnée ", "Étant donnés ", "Étant données ", "Etant donné que ",
                              "Etant donné qu'", "Etant donné ", "Etant donnée ", "Etant donnés ", "Etant données "},
                             {"Quand ", "Lorsque ", "Lorsqu'"},
                             {"Alors ", "Donc "},
                             {"Et que ", "Et qu'", "Et "},
                             {"Mais que ", "Mais qu'", "Mais "}};
    if (langue == "en") {
        return en;
    }
    if (langue == "fr") {
        return fr;
    }
    throw ErreurGherkin(fichier, 1, "langue non prise en charge : " + langue + " (en, fr)");
}

std::string rogner(std::string_view s) {
    const auto debut = s.find_first_not_of(" \t\r");
    if (debut == std::string_view::npos) {
        return {};
    }
    const auto fin = s.find_last_not_of(" \t\r");
    return std::string(s.substr(debut, fin - debut + 1));
}

bool commencePar(const std::string& s, const std::string& prefixe) {
    return s.size() >= prefixe.size() && s.compare(0, prefixe.size(), prefixe) == 0;
}

// « Mot-clé: titre » : renvoie le titre si la ligne commence par l'un des mots-clés suivi de ':'.
std::optional<std::string> titreApres(const std::string& ligne, const std::vector<std::string>& mots) {
    for (const auto& mot : mots) {
        if (commencePar(ligne, mot + ":")) {
            return rogner(std::string_view(ligne).substr(mot.size() + 1));
        }
    }
    return std::nullopt;
}

// Plus long mot-clé d'étape qui préfixe la ligne.
std::optional<std::string> motCleEtape(const std::string& ligne, const std::vector<std::string>& mots) {
    std::optional<std::string> meilleur;
    for (const auto& mot : mots) {
        if (commencePar(ligne, mot) && (!meilleur || mot.size() > meilleur->size())) {
            meilleur = mot;
        }
    }
    return meilleur;
}

// Cellules d'une ligne de table : | a | b \| c | -> {"a", "b | c"} (échappements \| \\ \n).
std::vector<std::string> cellules(const std::string& ligne, const std::string& fichier, int numero) {
    if (ligne.size() < 2 || ligne.back() != '|') {
        throw ErreurGherkin(fichier, numero, "ligne de table non terminée par « | »");
    }
    std::vector<std::string> resultat;
    std::string courante;
    for (std::size_t i = 1; i < ligne.size(); ++i) {
        const char c = ligne[i];
        if (c == '\\' && i + 1 < ligne.size()) {
            const char suivant = ligne[++i];
            courante += suivant == 'n' ? '\n' : suivant;
        } else if (c == '|') {
            resultat.push_back(rogner(courante));
            courante.clear();
        } else {
            courante += c;
        }
    }
    return resultat;
}

class Analyse {
public:
    Analyse(std::string_view texte, std::string fichier) : fichier_(std::move(fichier)) {
        std::string ligne;
        std::istringstream flux{std::string(texte)};
        while (std::getline(flux, ligne)) {
            if (lignes_.empty() && commencePar(ligne, "\xEF\xBB\xBF")) {
                ligne.erase(0, 3);   // BOM UTF-8
            }
            lignes_.push_back(ligne);
        }
    }

    Fonctionnalite executer() {
        f_.fichier = fichier_;
        detecterLangue();
        const MotsCles& m = motsCles(f_.langue, fichier_);
        for (std::size_t i = 0; i < lignes_.size(); ++i) {
            numero_ = static_cast<int>(i) + 1;
            if (docString_) {
                continuerDocString(lignes_[i]);
                continue;
            }
            traiterLigne(lignes_[i], m);
        }
        if (docString_) {
            throw ErreurGherkin(fichier_, docString_->debut, "doc string non refermée");
        }
        if (!fonctionnaliteVue_) {
            throw ErreurGherkin(fichier_, 1, "aucune Fonctionnalité (Feature) dans le fichier");
        }
        verifierPlans();
        return std::move(f_);
    }

private:
    struct DocStringEnCours {
        std::string delimiteur;
        std::size_t retrait;
        std::vector<std::string> lignes;
        int debut;
    };

    void detecterLangue() {
        for (const auto& brute : lignes_) {
            const std::string l = rogner(brute);
            if (l.empty()) {
                continue;
            }
            if (!commencePar(l, "#")) {
                return;
            }
            std::string commentaire = rogner(std::string_view(l).substr(1));
            if (commencePar(commentaire, "language:")) {
                f_.langue = rogner(std::string_view(commentaire).substr(9));
                return;
            }
        }
    }

    void erreur(const std::string& message) const { throw ErreurGherkin(fichier_, numero_, message); }

    std::vector<Etape>* etapesCourantes() {
        if (scenario_) {
            return &scenario_->etapes;
        }
        return dansContexte_ ? &f_.contexte : nullptr;
    }

    Etape* derniereEtape() {
        auto* etapes = etapesCourantes();
        return (etapes && !etapes->empty() && etapeRecente_) ? &etapes->back() : nullptr;
    }

    void traiterLigne(const std::string& brute, const MotsCles& m) {
        const std::string l = rogner(brute);
        if (l.empty() || commencePar(l, "#")) {
            return;
        }
        if (commencePar(l, "@")) {
            std::istringstream mots(l);
            for (std::string tag; mots >> tag;) {
                if (commencePar(tag, "#")) {
                    break;   // commentaire en fin de ligne
                }
                if (!commencePar(tag, "@")) {
                    erreur("tag invalide : " + tag);
                }
                tagsEnAttente_.push_back(tag);
            }
            return;
        }
        if (commencePar(l, "|")) {
            ajouterLigneDeTable(cellules(l, fichier_, numero_));
            return;
        }
        if (commencePar(l, "\"\"\"") || commencePar(l, "```")) {
            if (!derniereEtape()) {
                erreur("doc string sans étape");
            }
            docString_ = DocStringEnCours{l.substr(0, 3), brute.find_first_not_of(" \t"), {}, numero_};
            return;
        }
        if (auto titre = titreApres(l, m.fonctionnalite)) {
            if (fonctionnaliteVue_) {
                erreur("une seule Fonctionnalité par fichier");
            }
            fonctionnaliteVue_ = true;
            f_.nom = *titre;
            f_.ligne = numero_;
            f_.tags = std::exchange(tagsEnAttente_, {});
            descriptionFonctionnalite_ = true;
            return;
        }
        if (!fonctionnaliteVue_) {
            erreur("la première instruction doit être « Feature: » ou « Fonctionnalité: »");
        }
        if (auto titre = titreApres(l, m.contexte)) {
            (void)titre;
            nouveauBloc();
            dansContexte_ = true;
            return;
        }
        if (auto titre = titreApres(l, m.plan)) {
            nouveauScenario(l, *titre, true);
            return;
        }
        if (auto titre = titreApres(l, m.scenario)) {
            nouveauScenario(l, *titre, false);
            return;
        }
        if (auto titre = titreApres(l, m.exemples)) {
            if (!scenario_ || !scenario_->plan) {
                erreur("« Exemples » / « Examples » hors d'un plan du scénario");
            }
            scenario_->exemples.push_back(Exemples{*titre, std::exchange(tagsEnAttente_, {}), {}, numero_});
            exemples_ = &scenario_->exemples.back();
            etapeRecente_ = false;
            return;
        }
        if (auto titre = titreApres(l, m.regle)) {
            (void)titre;
            nouveauBloc();   // une règle regroupe des scénarios : rien de plus ici
            tagsEnAttente_.clear();
            return;
        }
        if (traiterEtape(l, m)) {
            return;
        }
        // Texte libre : description de la fonctionnalité, ou d'un scénario avant sa première étape.
        if (descriptionFonctionnalite_) {
            f_.description += (f_.description.empty() ? "" : "\n") + l;
            return;
        }
        if ((scenario_ && scenario_->etapes.empty() && !exemples_) || (dansContexte_ && f_.contexte.empty())) {
            return;
        }
        erreur("ligne inattendue : « " + l + " »");
    }

    bool traiterEtape(const std::string& l, const MotsCles& m) {
        struct Famille {
            const std::vector<std::string>* mots;
            std::optional<TypeEtape> type;
        };
        const std::vector<std::string> etoile{"* "};
        const Famille familles[] = {{&m.soit, TypeEtape::Contexte},  {&m.quand, TypeEtape::Action},
                                    {&m.alors, TypeEtape::Resultat}, {&m.et, std::nullopt},
                                    {&m.mais, std::nullopt},         {&etoile, std::nullopt}};
        std::optional<std::string> mot;
        std::optional<TypeEtape> type;
        for (const auto& famille : familles) {
            if (auto trouve = motCleEtape(l, *famille.mots); trouve && (!mot || trouve->size() > mot->size())) {
                mot = trouve;
                type = famille.type;
            }
        }
        if (!mot) {
            return false;
        }
        auto* etapes = etapesCourantes();
        if (!etapes) {
            erreur("étape hors d'un scénario ou d'un contexte");
        }
        if (exemples_) {
            erreur("étape après les Exemples");
        }
        descriptionFonctionnalite_ = false;
        Etape e;
        e.motCle = *mot;
        e.texte = rogner(std::string_view(l).substr(mot->size()));
        e.type = type.value_or(dernierType_);
        e.ligne = numero_;
        dernierType_ = e.type;
        etapes->push_back(std::move(e));
        etapeRecente_ = true;
        return true;
    }

    void ajouterLigneDeTable(std::vector<std::string> ligne) {
        Table* table = nullptr;
        if (exemples_) {
            table = &exemples_->table;
        } else if (Etape* etape = derniereEtape()) {
            if (!etape->table) {
                etape->table = Table{};
            }
            table = &*etape->table;
        } else {
            erreur("table de données sans étape ni exemples");
        }
        if (!table->vide() && table->lignes().front().size() != ligne.size()) {
            erreur("nombre de cellules différent de la première ligne de la table");
        }
        table->lignes().push_back(std::move(ligne));
    }

    void continuerDocString(const std::string& brute) {
        if (rogner(brute) == docString_->delimiteur) {
            std::string contenu;
            for (std::size_t i = 0; i < docString_->lignes.size(); ++i) {
                contenu += (i ? "\n" : "") + docString_->lignes[i];
            }
            derniereEtape()->docString = std::move(contenu);
            docString_.reset();
            return;
        }
        // Retire l'indentation du délimiteur d'ouverture.
        std::size_t retrait = 0;
        while (retrait < docString_->retrait && retrait < brute.size() && (brute[retrait] == ' ' || brute[retrait] == '\t')) {
            ++retrait;
        }
        std::string ligne = brute.substr(retrait);
        if (!ligne.empty() && ligne.back() == '\r') {
            ligne.pop_back();
        }
        docString_->lignes.push_back(std::move(ligne));
    }

    void nouveauBloc() {
        scenario_ = nullptr;
        exemples_ = nullptr;
        dansContexte_ = false;
        etapeRecente_ = false;
        descriptionFonctionnalite_ = false;
    }

    void nouveauScenario(const std::string& ligne, const std::string& titre, bool plan) {
        nouveauBloc();
        Scenario sc;
        sc.motCle = ligne.substr(0, ligne.find(':'));
        sc.nom = titre;
        sc.plan = plan;
        sc.ligne = numero_;
        sc.tags = std::exchange(tagsEnAttente_, {});
        f_.scenarios.push_back(std::move(sc));
        scenario_ = &f_.scenarios.back();
    }

    void verifierPlans() const {
        for (const auto& sc : f_.scenarios) {
            if (!sc.plan) {
                continue;
            }
            if (sc.exemples.empty()) {
                throw ErreurGherkin(fichier_, sc.ligne, "plan du scénario sans Exemples : " + sc.nom);
            }
            for (const auto& ex : sc.exemples) {
                if (ex.table.lignes().size() < 2) {
                    throw ErreurGherkin(fichier_, ex.ligne, "Exemples sans en-tête ou sans ligne de données");
                }
            }
        }
    }

    std::string fichier_;
    std::vector<std::string> lignes_;
    Fonctionnalite f_;
    int numero_ = 0;
    bool fonctionnaliteVue_ = false;
    bool descriptionFonctionnalite_ = false;
    bool dansContexte_ = false;
    bool etapeRecente_ = false;
    Scenario* scenario_ = nullptr;
    Exemples* exemples_ = nullptr;
    TypeEtape dernierType_ = TypeEtape::Contexte;
    std::vector<std::string> tagsEnAttente_;
    std::optional<DocStringEnCours> docString_;
};

} // namespace

Fonctionnalite analyser(std::string_view texte, const std::string& fichier) {
    return Analyse(texte, fichier).executer();
}

Fonctionnalite lireFichier(const std::filesystem::path& chemin) {
    std::ifstream entree(chemin, std::ios::binary);
    if (!entree) {
        throw ErreurGherkin(chemin.generic_string(), 0, "fichier illisible");
    }
    std::ostringstream contenu;
    contenu << entree.rdbuf();
    return analyser(contenu.str(), chemin.generic_string());
}

std::vector<std::filesystem::path> trouverFichiers(const std::vector<std::filesystem::path>& chemins) {
    namespace fs = std::filesystem;
    std::vector<fs::path> resultat;
    for (const auto& chemin : chemins) {
        if (fs::is_directory(chemin)) {
            for (const auto& entree : fs::recursive_directory_iterator(chemin)) {
                if (entree.is_regular_file() && entree.path().extension() == ".feature") {
                    resultat.push_back(entree.path());
                }
            }
        } else if (fs::exists(chemin)) {
            resultat.push_back(chemin);
        } else {
            throw ErreurGherkin(chemin.generic_string(), 0, "fichier ou dossier introuvable");
        }
    }
    // Chemins relatifs au dossier courant quand c'est possible : messages plus lisibles.
    for (auto& chemin : resultat) {
        std::error_code erreur;
        if (auto relatif = fs::proximate(chemin, erreur); !erreur) {
            chemin = relatif;
        }
    }
    std::sort(resultat.begin(), resultat.end());
    return resultat;
}

} // namespace gherkin
