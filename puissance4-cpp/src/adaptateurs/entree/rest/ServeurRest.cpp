// Dernière génération/modification faite par l'IA Claude le 13/10/2026 19:30:35
#include "adaptateurs/entree/rest/ServeurRest.hpp"

#include "adaptateurs/entree/rest/PartieJson.hpp"
#include "application/Erreurs.hpp"
#include "domaine/Erreurs.hpp"

#include <httplib.h>
#include <nlohmann/json.hpp>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>

namespace p4::adaptateurs {

namespace {

constexpr const char* TYPE_JSON = "application/json; charset=utf-8";

void repondreErreur(httplib::Response& reponse, int statut, const std::string& message) {
    reponse.status = statut;
    reponse.set_content(nlohmann::json{{"erreur", message}}.dump(), TYPE_JSON);
}

} // namespace

ServeurRest::ServeurRest(application::CreerPartie& creerPartie,
                         application::ConsulterPartie& consulterPartie,
                         application::JouerCoup& jouerCoup,
                         std::string dossierWeb,
                         InformationsVersion version)
    : creerPartie_(creerPartie),
      consulterPartie_(consulterPartie),
      jouerCoup_(jouerCoup),
      dossierWeb_(std::move(dossierWeb)),
      version_(std::move(version)),
      serveur_(std::make_unique<httplib::Server>()) {
    declarerRoutes();
}

ServeurRest::~ServeurRest() = default;

void ServeurRest::declarerRoutes() {
    // Sans TCP_NODELAY, l'algorithme de Nagle et l'ACK retardé du client ajoutent
    // ~40 ms à chaque petite réponse (médiane mesurée par k6 : 43 ms au lieu de quelques ms).
    serveur_->set_tcp_nodelay(true);
    serveur_->set_default_headers({{"Access-Control-Allow-Origin", "*"}});

    // EX-21 : le point de santé identifie le binaire (version, commit, build de la CI)
    serveur_->Get("/api/sante", [this](const httplib::Request&, httplib::Response& reponse) {
        reponse.set_content(nlohmann::json{{"statut", "OK"},
                                           {"version", version_.version},
                                           {"commit", version_.commit},
                                           {"build", version_.build}}.dump(),
                            TYPE_JSON);
    });

    serveur_->Post("/api/parties", [this](const httplib::Request& requete, httplib::Response& reponse) {
        const auto corps = nlohmann::json::parse(requete.body, nullptr, /*exceptions*/ false);
        if (corps.is_discarded() || !corps.is_object()) {
            repondreErreur(reponse, 400, "Corps JSON invalide");
            return;
        }
        try {
            application::DemandeCreationPartie demande{
                corps.value("joueur1", std::string{}),
                corps.value("joueur2", std::string{})};
            const domaine::Partie partie = creerPartie_.creer(demande);
            reponse.status = 201;
            reponse.set_header("Location", "/api/parties/" + partie.id());
            reponse.set_content(versJson(partie).dump(), TYPE_JSON);
        } catch (const nlohmann::json::exception&) {
            repondreErreur(reponse, 400, "Les champs joueur1 et joueur2 doivent être des chaînes");
        } catch (const std::invalid_argument& erreur) {
            repondreErreur(reponse, 400, erreur.what());
        }
    });

    serveur_->Get(R"(/api/parties/([A-Za-z0-9_-]+))",
                  [this](const httplib::Request& requete, httplib::Response& reponse) {
        const auto partie = consulterPartie_.consulter(requete.matches[1].str());
        if (!partie) {
            repondreErreur(reponse, 404, "Partie inconnue : " + requete.matches[1].str());
            return;
        }
        reponse.set_content(versJson(*partie).dump(), TYPE_JSON);
    });

    // Itération 2 : jouer un coup (EX-06 à EX-11)
    serveur_->Post(R"(/api/parties/([A-Za-z0-9_-]+)/coups)",
                   [this](const httplib::Request& requete, httplib::Response& reponse) {
        const auto corps = nlohmann::json::parse(requete.body, nullptr, /*exceptions*/ false);
        // EX-08 exige un ENTIER : seuls les entiers JSON représentables en int sont acceptés
        // (convertir un nombre hors de la plage d'int serait un comportement indéfini).
        if (corps.is_discarded() || !corps.is_object() || !corps.contains("colonne") ||
            !corps["colonne"].is_number_integer()) {
            repondreErreur(reponse, 400, "Colonne invalide : doit être un entier entre 0 et 6");
            return;
        }
        const auto valeur = corps["colonne"].get<std::int64_t>();
        if (valeur < std::numeric_limits<int>::min() || valeur > std::numeric_limits<int>::max()) {
            repondreErreur(reponse, 400, "Colonne invalide : doit être un entier entre 0 et 6");
            return;
        }
        try {
            const int colonne = static_cast<int>(valeur);
            const domaine::Partie partie = jouerCoup_.jouer(requete.matches[1].str(), colonne);
            reponse.set_content(versJson(partie).dump(), TYPE_JSON);
        } catch (const application::PartieInconnue& erreur) {
            repondreErreur(reponse, 404, erreur.what());
        } catch (const domaine::ColonnePleine& erreur) {
            repondreErreur(reponse, 409, erreur.what());
        } catch (const domaine::PartieTerminee& erreur) {  // EX-17
            repondreErreur(reponse, 409, erreur.what());
        } catch (const std::invalid_argument& erreur) {
            repondreErreur(reponse, 400, erreur.what());
        }
    });

    if (!dossierWeb_.empty()) {
        serveur_->set_mount_point("/", dossierWeb_);
    }
}

void ServeurRest::definirNombreDeThreads(std::size_t nombre) {
    serveur_->new_task_queue = [nombre] { return new httplib::ThreadPool(nombre); };
}

int ServeurRest::lier(const std::string& hote, int port) {
    if (port == 0) {
        return serveur_->bind_to_any_port(hote);
    }
    return serveur_->bind_to_port(hote, port) ? port : -1;
}

bool ServeurRest::ecouter() { return serveur_->listen_after_bind(); }

void ServeurRest::arreter() { serveur_->stop(); }

bool ServeurRest::estDemarre() const { return serveur_->is_running(); }

} // namespace p4::adaptateurs
