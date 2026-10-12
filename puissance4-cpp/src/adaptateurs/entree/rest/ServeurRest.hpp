// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#pragma once

#include "application/ports/entree/ConsulterPartie.hpp"
#include "application/ports/entree/CreerPartie.hpp"

#include <memory>
#include <string>

namespace httplib { class Server; }

namespace p4::adaptateurs {

// ADAPTATEUR D'ENTRÉE : traduit HTTP/JSON en appels aux ports d'entrée.
//   GET  /api/sante            -> 200 {"statut":"OK", "version":...}
//   POST /api/parties          -> 201 + Location (400 si demande invalide)
//   GET  /api/parties/{id}     -> 200 (404 si inconnue)
//   GET  /                     -> IHM HTML5 (fichiers statiques de dossierWeb)
class ServeurRest {
public:
    ServeurRest(application::CreerPartie& creerPartie,
                application::ConsulterPartie& consulterPartie,
                std::string dossierWeb = {});
    ~ServeurRest();

    ServeurRest(const ServeurRest&) = delete;
    ServeurRest& operator=(const ServeurRest&) = delete;

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
    std::string dossierWeb_;
    std::unique_ptr<httplib::Server> serveur_;
};

} // namespace p4::adaptateurs
