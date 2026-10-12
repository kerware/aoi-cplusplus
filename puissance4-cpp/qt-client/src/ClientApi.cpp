// Dernière génération/modification faite par l'IA Claude le 09/10/2026 17:45:07
#include "ClientApi.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <utility>

ClientApi::ClientApi(QUrl urlServeur, QObject* parent) : QObject(parent), urlServeur_(std::move(urlServeur)) {}

void ClientApi::creerPartie(const QString& joueur1, const QString& joueur2) {
    const QJsonObject corps{{"joueur1", joueur1}, {"joueur2", joueur2}};
    envoyer(QStringLiteral("/api/parties"), QJsonDocument(corps).toJson(QJsonDocument::Compact));
}

void ClientApi::jouer(const QString& idPartie, int colonne) {
    const QJsonObject corps{{"colonne", colonne}};
    envoyer(QStringLiteral("/api/parties/%1/coups").arg(idPartie), QJsonDocument(corps).toJson(QJsonDocument::Compact));
}

void ClientApi::envoyer(const QString& chemin, const QByteArray& corps) {
    QUrl url = urlServeur_;
    url.setPath(chemin);
    QNetworkRequest requete(url);
    requete.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    requete.setTransferTimeout(5000);
    QNetworkReply* reponse = reseau_.post(requete, corps);
    connect(reponse, &QNetworkReply::finished, this, [this, reponse] {
        reponse->deleteLater();
        const QByteArray contenu = reponse->readAll();
        const int statut = reponse->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statut == 0) {
            emit erreur(QStringLiteral("Serveur injoignable : %1").arg(reponse->errorString()));
            return;
        }
        if (statut >= 400) {
            emit erreur(lireErreur(contenu));
            return;
        }
        const auto partie = lirePartie(contenu);
        if (!partie) {
            emit erreur(QStringLiteral("Réponse illisible du serveur"));
            return;
        }
        emit partieRecue(*partie);
    });
}
