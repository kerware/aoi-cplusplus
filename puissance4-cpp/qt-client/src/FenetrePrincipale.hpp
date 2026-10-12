// Dernière génération/modification faite par l'IA Claude le 09/10/2026 17:45:07
#pragma once

#include "ClientApi.hpp"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

#include <array>
#include <optional>

// Fenêtre du client Qt. Chaque widget a un objectName stable (équivalent des data-testid
// de l'IHM web) : les tests QtTest le retrouvent par findChild<>().
class FenetrePrincipale : public QWidget {
    Q_OBJECT

public:
    explicit FenetrePrincipale(ClientApi& client, QWidget* parent = nullptr);

    const std::optional<PartieVue>& partie() const { return partie_; }

private:
    void afficherPartie(const PartieVue& partie);
    void afficherErreur(const QString& message);

    ClientApi& client_;
    QLineEdit* joueur1_;
    QLineEdit* joueur2_;
    QLabel* message_;
    QLabel* etat_;
    std::array<QPushButton*, 7> colonnes_{};
    std::array<std::array<QLabel*, 7>, 6> cases_{};
    std::optional<PartieVue> partie_;
};
