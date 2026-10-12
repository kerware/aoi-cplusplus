// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#include "application/ServicePartie.hpp"

namespace p4::application {

ServicePartie::ServicePartie(PartieRepository& repository, GenerateurIdentifiant& generateur)
    : repository_(repository), generateur_(generateur) {}

domaine::Partie ServicePartie::creer(const DemandeCreationPartie& demande) {
    // Le constructeur de Partie valide les noms : en cas d'erreur,
    // l'exception remonte et rien n'est sauvegardé.
    domaine::Partie partie(generateur_.suivant(), demande.nomJoueur1, demande.nomJoueur2);
    repository_.sauvegarder(partie);
    return partie;
}

std::optional<domaine::Partie> ServicePartie::consulter(const domaine::IdPartie& id) const {
    return repository_.trouverParId(id);
}

} // namespace p4::application
