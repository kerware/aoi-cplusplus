// Dernière génération/modification faite par l'IA Claude le 09/10/2026 17:45:07
// Tests UNITAIRES QtTest : lecture du JSON de l'API et texte d'état (EX-18).
// Aucun réseau, aucune fenêtre : rapides et déterministes.
#include "PartieVue.hpp"

#include <QtTest>

class TestPartieVue : public QObject {
    Q_OBJECT

private slots:
    void lit_une_partie_en_cours() {
        const QByteArray json = R"({"id":"P0007","statut":"EN_COURS","joueurCourant":"Bob",
            "joueur1":{"nom":"Alice","couleur":"ROUGE"},"joueur2":{"nom":"Bob","couleur":"JAUNE"},
            "vainqueur":null,"nombreDeCoups":1,
            "grille":[".......",".......",".......",".......",".......","...R..."]})";

        const auto partie = lirePartie(json);

        QVERIFY(partie.has_value());
        QCOMPARE(partie->id, QStringLiteral("P0007"));
        QCOMPARE(partie->joueurCourant, QStringLiteral("Bob"));
        QVERIFY(partie->vainqueur.isEmpty());
        QCOMPARE(partie->grille.size(), 6);
        QCOMPARE(partie->grille[5][3], QLatin1Char('R'));
    }

    void refuse_un_json_invalide() {
        QVERIFY(!lirePartie("pas du json").has_value());
        QVERIFY(!lirePartie(R"({"erreur":"Partie inconnue"})").has_value());
    }

    void lit_le_message_d_erreur_de_l_api() {
        QCOMPARE(lireErreur(R"({"erreur":"Colonne pleine : 0"})"), QStringLiteral("Colonne pleine : 0"));
        QCOMPARE(lireErreur("<html>"), QStringLiteral("Réponse inattendue du serveur"));
    }

    // Test piloté par les données : une ligne = un cas (équivalent de TEST_P en GoogleTest)
    void texte_d_etat_data() {
        QTest::addColumn<QString>("statut");
        QTest::addColumn<QString>("vainqueur");
        QTest::addColumn<QString>("attendu");
        QTest::newRow("en cours") << "EN_COURS" << "" << "Au tour de Alice";
        QTest::newRow("gagnée") << "GAGNEE" << "Bob" << "Partie terminée — victoire de Bob";
        QTest::newRow("nulle") << "NULLE" << "" << "Partie terminée — match nul";
    }

    void texte_d_etat() {
        QFETCH(QString, statut);
        QFETCH(QString, vainqueur);
        QFETCH(QString, attendu);
        PartieVue partie;
        partie.statut = statut;
        partie.vainqueur = vainqueur;
        partie.joueurCourant = QStringLiteral("Alice");

        QCOMPARE(texteEtat(partie), attendu);
    }

    void une_colonne_pleine_ou_une_partie_finie_n_est_pas_jouable() {
        PartieVue partie;
        partie.statut = QStringLiteral("EN_COURS");
        partie.grille = QStringList{"R......", ".......", ".......", ".......", ".......", "J......"};
        QVERIFY(!partie.colonneJouable(0));
        QVERIFY(partie.colonneJouable(1));
        QVERIFY(!partie.colonneJouable(7));

        partie.statut = QStringLiteral("GAGNEE");
        QVERIFY(!partie.colonneJouable(1));
    }
};

QTEST_GUILESS_MAIN(TestPartieVue)
#include "tst_partievue.moc"
