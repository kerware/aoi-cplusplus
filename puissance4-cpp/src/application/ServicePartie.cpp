// Dernière génération/modification faite par l'IA Claude le 13/10/2026 19:29:37
#include "application/ServicePartie.hpp"

#include "application/Erreurs.hpp"

namespace p4::application {

ServicePartie::ServicePartie(PartieRepository& repository, GenerateurIdentifiant& generateur)
    : repository_(repository), generateur_(generateur) {}

domaine::Partie ServicePartie::creer(const DemandeCreationPartie& demande) {
    // Valider AVANT de consommer un identifiant : pas de trou dans la numérotation (EX-04).
    domaine::Partie::verifierNoms(demande.nomJoueur1, demande.nomJoueur2);

    domaine::Partie partie(generateur_.suivant(), demande.nomJoueur1, demande.nomJoueur2);
    repository_.sauvegarder(partie);
    return partie;
}

std::optional<domaine::Partie> ServicePartie::consulter(const domaine::IdPartie& id) const {
    return repository_.trouverParId(id);
}

domaine::Partie ServicePartie::jouer(const domaine::IdPartie& id, int colonne) {
    const std::lock_guard verrou(verrouCoups_);  // EX-19
    std::optional<domaine::Partie> partie = repository_.trouverParId(id);
    if (!partie) {
        throw PartieInconnue(id);
    }
    partie->jouer(colonne);  // lève si le coup est refusé : rien n'est sauvegardé
    repository_.sauvegarder(*partie);
    return *partie;
}

} // namespace p4::application
