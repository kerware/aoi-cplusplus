// Dernière génération/modification faite par l'IA Claude le 13/10/2026 19:30:35
#pragma once

#include "application/ports/entree/ConsulterPartie.hpp"
#include "application/ports/entree/CreerPartie.hpp"
#include "application/ports/entree/JouerCoup.hpp"

#include <memory>
#include <string>

namespace httplib { class Server; }

namespace p4::adaptateurs {

// EX-21 : identification du binaire en service. Fournie par la composition root (main.cpp),
// renseignée à la compilation par la CI (commit Git, numéro de build).
struct InformationsVersion {
    std::string version = "0.0.0";
    std::string commit = "local";
    std::string build = "0";
};

// ADAPTATEUR D'ENTRÉE : traduit HTTP/JSON en appels aux ports d'entrée.
//   GET  /api/sante            -> 200 {"statut":"OK", "version":..., "commit":..., "build":...}
//   POST /api/parties          -> 201 + Location (400 si demande invalide)
//   GET  /api/parties/{id}     -> 200 (404 si inconnue)
//   POST /api/parties/{id}/coups {"colonne":3}
//                              -> 200 (400 colonne invalide, 404 partie inconnue, 409 colonne pleine)
//   GET  /                     -> IHM HTML5 (fichiers statiques de dossierWeb)
class ServeurRest {
public:
    ServeurRest(application::CreerPartie& creerPartie,
                application::ConsulterPartie& consulterPartie,
                application::JouerCoup& jouerCoup,
                std::string dossierWeb = {},
                InformationsVersion version = {});
    ~ServeurRest();

    ServeurRest(const ServeurRest&) = delete;
    ServeurRest& operator=(const ServeurRest&) = delete;

    // Nombre de threads de traitement des requêtes, à appeler avant lier().
    // Chaque connexion HTTP « keep-alive » occupe un thread : avec le défaut de cpp-httplib
    // (nombre de cœurs - 1), 50 clients simultanés attendent leur tour (p95 > 300 ms).
    void definirNombreDeThreads(std::size_t nombre);

    // Réserve le port demandé (0 = un port libre choisi par le système).
    // Retourne le port réellement utilisé, ou -1 en cas d'échec.
    int lier(const std::string& hote, int port);

    // Bloque jusqu'à l'appel de arreter() (depuis un autre thread).
    bool ecouter();

    void arreter();
    bool estDemarre() const;

private:
    void declarerRoutes();

    application::CreerPartie& creerPartie_;
    application::ConsulterPartie& consulterPartie_;
    application::JouerCoup& jouerCoup_;
    std::string dossierWeb_;
    InformationsVersion version_;
    std::unique_ptr<httplib::Server> serveur_;
};

} // namespace p4::adaptateurs
