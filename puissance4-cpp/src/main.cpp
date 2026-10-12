// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
// ---------------------------------------------------------------------------
// Point d'entrée = "composition root" de l'architecture hexagonale :
// c'est le SEUL endroit qui connaît à la fois le domaine et les adaptateurs.
//
// Usage : puissance4_serveur [--port 8080] [--hote 0.0.0.0] [--web <dossier>]
// Arrêt propre : taper "q" puis Entrée dans la console.
// ---------------------------------------------------------------------------
#include "adaptateurs/entree/rest/ServeurRest.hpp"
#include "adaptateurs/sortie/memoire/GenerateurIdentifiantSequentiel.hpp"
#include "adaptateurs/sortie/memoire/PartieRepositoryEnMemoire.hpp"
#include "application/ServicePartie.hpp"

#include <filesystem>
#include <iostream>
#include <string>
#include <thread>

namespace {

struct Configuration {
    std::string hote = "0.0.0.0";
    int port = 8080;
    std::string dossierWeb;
};

Configuration lireConfiguration(int argc, char* argv[]) {
    Configuration configuration;
    // Par défaut, l'IHM est cherchée dans <dossier de l'exécutable>/web
    configuration.dossierWeb =
        (std::filesystem::path(argv[0]).parent_path() / "web").string();
    for (int i = 1; i + 1 < argc; i += 2) {
        const std::string option = argv[i];
        const std::string valeur = argv[i + 1];
        if (option == "--port") {
            configuration.port = std::stoi(valeur);
        } else if (option == "--hote") {
            configuration.hote = valeur;
        } else if (option == "--web") {
            configuration.dossierWeb = valeur;
        }
    }
    if (!std::filesystem::is_directory(configuration.dossierWeb)) {
        std::cerr << "Avertissement : dossier web introuvable (" << configuration.dossierWeb
                  << "), seule l'API REST est servie.\n";
        configuration.dossierWeb.clear();
    }
    return configuration;
}

} // namespace

int main(int argc, char* argv[]) {
    const Configuration configuration = lireConfiguration(argc, argv);

    // 1. Adaptateurs de sortie
    p4::adaptateurs::PartieRepositoryEnMemoire repository;
    p4::adaptateurs::GenerateurIdentifiantSequentiel generateur;

    // 2. Cas d'utilisation (domaine), branché sur les ports de sortie
    p4::application::ServicePartie service(repository, generateur);

    // 3. Adaptateur d'entrée, branché sur les ports d'entrée
    p4::adaptateurs::ServeurRest serveur(service, service, configuration.dossierWeb);

    const int port = serveur.lier(configuration.hote, configuration.port);
    if (port < 0) {
        std::cerr << "Impossible d'écouter sur " << configuration.hote << ":" << configuration.port
                  << "\n";
        return 1;
    }
    std::cout << "Puissance 4 - serveur REST demarre sur http://localhost:" << port << "\n"
              << "Tapez q puis Entree pour arreter." << std::endl;

    // Arrêt propre sur "q" ; si l'entrée standard est fermée (Docker détaché),
    // le thread se termine et le serveur continue de tourner.
    std::thread console([&serveur] {
        std::string ligne;
        while (std::getline(std::cin, ligne)) {
            if (ligne == "q" || ligne == "quit") {
                serveur.arreter();
                return;
            }
        }
    });
    console.detach();

    serveur.ecouter();
    std::cout << "Serveur arrete." << std::endl;
    return 0;
}
