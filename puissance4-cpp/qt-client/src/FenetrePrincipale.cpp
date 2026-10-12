// Dernière génération/modification faite par l'IA Claude le 09/10/2026 17:45:07
#include "FenetrePrincipale.hpp"

#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace {

constexpr int TAILLE_CASE = 48;

QString styleCase(QChar symbole) {
    const char* couleur = symbole == QLatin1Char('R') ? "#d64545" : symbole == QLatin1Char('J') ? "#f2c230" : "#ffffff";
    return QStringLiteral("background:%1; border-radius:%2px;").arg(QLatin1String(couleur)).arg(TAILLE_CASE / 2);
}

} // namespace

FenetrePrincipale::FenetrePrincipale(ClientApi& client, QWidget* parent) : QWidget(parent), client_(client) {
    setWindowTitle(QStringLiteral("Puissance 4 — client Qt"));

    joueur1_ = new QLineEdit(QStringLiteral("Alice"));
    joueur1_->setObjectName("joueur1");
    joueur2_ = new QLineEdit(QStringLiteral("Bob"));
    joueur2_->setObjectName("joueur2");
    auto* nouvellePartie = new QPushButton(QStringLiteral("Nouvelle partie"));
    nouvellePartie->setObjectName("nouvellePartie");

    auto* formulaire = new QFormLayout;
    formulaire->addRow(QStringLiteral("Joueur 1 (rouge)"), joueur1_);
    formulaire->addRow(QStringLiteral("Joueur 2 (jaune)"), joueur2_);
    formulaire->addRow(nouvellePartie);

    message_ = new QLabel;
    message_->setObjectName("message");
    etat_ = new QLabel;
    etat_->setObjectName("etat");
    etat_->setStyleSheet(QStringLiteral("font-weight:bold;"));

    auto* plateau = new QWidget;
    plateau->setStyleSheet(QStringLiteral("background:#2f5897; border-radius:10px;"));
    auto* grille = new QGridLayout(plateau);
    grille->setSpacing(6);
    for (int c = 0; c < 7; ++c) {
        auto* bouton = new QPushButton(QStringLiteral("▼"));
        bouton->setObjectName(QStringLiteral("colonne-%1").arg(c));
        bouton->setEnabled(false);
        bouton->setStyleSheet(QStringLiteral("background:#e3ebf6; color:#2f5897; border-radius:4px; padding:4px;"));
        connect(bouton, &QPushButton::clicked, this, [this, c] {
            if (partie_) {
                client_.jouer(partie_->id, c);
            }
        });
        colonnes_[c] = bouton;
        grille->addWidget(bouton, 0, c);
    }
    for (int l = 0; l < 6; ++l) {
        for (int c = 0; c < 7; ++c) {
            auto* caseGrille = new QLabel;
            caseGrille->setObjectName(QStringLiteral("case-%1-%2").arg(l).arg(c));
            caseGrille->setFixedSize(TAILLE_CASE, TAILLE_CASE);
            caseGrille->setStyleSheet(styleCase(QLatin1Char('.')));
            cases_[l][c] = caseGrille;
            grille->addWidget(caseGrille, l + 1, c);
        }
    }

    auto* disposition = new QVBoxLayout(this);
    disposition->addLayout(formulaire);
    disposition->addWidget(message_);
    disposition->addWidget(etat_);
    disposition->addWidget(plateau);

    connect(nouvellePartie, &QPushButton::clicked, this,
            [this] { client_.creerPartie(joueur1_->text(), joueur2_->text()); });
    connect(&client_, &ClientApi::partieRecue, this, &FenetrePrincipale::afficherPartie);
    connect(&client_, &ClientApi::erreur, this, &FenetrePrincipale::afficherErreur);
}

void FenetrePrincipale::afficherPartie(const PartieVue& partie) {
    partie_ = partie;
    message_->setStyleSheet(QString());
    message_->setText(QStringLiteral("Partie %1 — %2 coup(s)").arg(partie.id).arg(partie.nombreDeCoups));
    etat_->setText(texteEtat(partie));
    for (int c = 0; c < 7; ++c) {
        colonnes_[c]->setEnabled(partie.colonneJouable(c));
    }
    for (int l = 0; l < 6 && l < partie.grille.size(); ++l) {
        for (int c = 0; c < 7 && c < partie.grille[l].size(); ++c) {
            cases_[l][c]->setStyleSheet(styleCase(partie.grille[l][c]));
            cases_[l][c]->setProperty("jeton", QString(partie.grille[l][c]));
        }
    }
}

void FenetrePrincipale::afficherErreur(const QString& message) {
    message_->setStyleSheet(QStringLiteral("color:#d64545; font-weight:bold;"));
    message_->setText(message);
}
