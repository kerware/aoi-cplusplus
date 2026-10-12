// Dernière génération/modification faite par l'IA Claude le 09/10/2026 17:45:07
#include "PartieVue.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

bool PartieVue::colonneJouable(int colonne) const {
    return enCours() && !grille.isEmpty() && colonne >= 0 && colonne < grille.first().size() &&
           grille.first().at(colonne) == QLatin1Char('.');
}

std::optional<PartieVue> lirePartie(const QByteArray& json) {
    const QJsonDocument document = QJsonDocument::fromJson(json);
    if (!document.isObject()) {
        return std::nullopt;
    }
    const QJsonObject objet = document.object();
    if (!objet.value("id").isString() || !objet.value("grille").isArray()) {
        return std::nullopt;
    }
    PartieVue partie;
    partie.id = objet.value("id").toString();
    partie.statut = objet.value("statut").toString();
    partie.joueur1 = objet.value("joueur1").toObject().value("nom").toString();
    partie.joueur2 = objet.value("joueur2").toObject().value("nom").toString();
    partie.joueurCourant = objet.value("joueurCourant").toString();
    partie.vainqueur = objet.value("vainqueur").toString();  // null -> chaîne vide
    partie.nombreDeCoups = objet.value("nombreDeCoups").toInt();
    for (const QJsonValue& ligne : objet.value("grille").toArray()) {
        partie.grille.append(ligne.toString());
    }
    return partie;
}

QString lireErreur(const QByteArray& json) {
    const QString erreur = QJsonDocument::fromJson(json).object().value("erreur").toString();
    return erreur.isEmpty() ? QStringLiteral("Réponse inattendue du serveur") : erreur;
}

QString texteEtat(const PartieVue& partie) {
    if (partie.statut == QLatin1String("GAGNEE")) {
        return QStringLiteral("Partie terminée — victoire de %1").arg(partie.vainqueur);
    }
    if (partie.statut == QLatin1String("NULLE")) {
        return QStringLiteral("Partie terminée — match nul");
    }
    return QStringLiteral("Au tour de %1").arg(partie.joueurCourant);
}
