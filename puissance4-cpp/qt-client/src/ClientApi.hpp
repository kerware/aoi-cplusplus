// Dernière génération/modification faite par l'IA Claude le 09/10/2026 17:45:07
#pragma once

#include "PartieVue.hpp"

#include <QNetworkAccessManager>
#include <QObject>
#include <QUrl>

// Client de l'API REST : requêtes asynchrones, résultats livrés par signaux
// (l'IHM ne se bloque jamais pendant un appel réseau).
class ClientApi : public QObject {
    Q_OBJECT

public:
    explicit ClientApi(QUrl urlServeur, QObject* parent = nullptr);

    void creerPartie(const QString& joueur1, const QString& joueur2);
    void jouer(const QString& idPartie, int colonne);

signals:
    void partieRecue(const PartieVue& partie);
    void erreur(const QString& message);

private:
    void envoyer(const QString& chemin, const QByteArray& corps);

    QUrl urlServeur_;
    QNetworkAccessManager reseau_;
};
