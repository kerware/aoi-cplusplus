// Dernière génération/modification faite par l'IA Claude le 09/10/2026 17:45:07
#pragma once

#include <QByteArray>
#include <QString>
#include <QStringList>

#include <optional>

// Ce que le client Qt sait d'une partie : la traduction de la réponse JSON de l'API.
// Aucune règle du jeu ici — elles sont dans le serveur (domaine).
struct PartieVue {
    QString id;
    QString statut;          // EN_COURS, GAGNEE, NULLE
    QString joueur1;
    QString joueur2;
    QString joueurCourant;
    QString vainqueur;       // vide si pas de vainqueur
    int nombreDeCoups = 0;
    QStringList grille;      // 6 lignes de 7 caractères : '.', 'R', 'J'

    bool enCours() const { return statut == QLatin1String("EN_COURS"); }
    bool colonneJouable(int colonne) const;
};

// std::nullopt si le JSON est invalide ou incomplet.
std::optional<PartieVue> lirePartie(const QByteArray& json);

// Message d'erreur renvoyé par l'API ({"erreur": "..."}), ou texte générique.
QString lireErreur(const QByteArray& json);

// EX-18 : « au tour de X », « Partie terminée — victoire de X » ou « Partie terminée — match nul ».
QString texteEtat(const PartieVue& partie);
