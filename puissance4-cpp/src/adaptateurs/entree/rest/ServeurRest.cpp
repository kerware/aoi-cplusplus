// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#include "adaptateurs/entree/rest/ServeurRest.hpp"

#include "adaptateurs/entree/rest/PartieJson.hpp"

#include <httplib.h>
#include <nlohmann/json.hpp>

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
                         std::string dossierWeb)
    : creerPartie_(creerPartie),
      consulterPartie_(consulterPartie),
      dossierWeb_(std::move(dossierWeb)),
      serveur_(std::make_unique<httplib::Server>()) {
    declarerRoutes();
}

ServeurRest::~ServeurRest() = default;

void ServeurRest::declarerRoutes() {
    serveur_->set_default_headers({{"Access-Control-Allow-Origin", "*"}});

    serveur_->Get("/api/sante", [](const httplib::Request&, httplib::Response& reponse) {
        reponse.set_content(nlohmann::json{{"statut", "OK"}, {"version", "1.0.0-iteration1"}}.dump(),
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

    if (!dossierWeb_.empty()) {
        serveur_->set_mount_point("/", dossierWeb_);
    }
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
