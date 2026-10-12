// Dernière génération/modification faite par l'IA Claude le 09/10/2026 17:45:07
// Test d'IHM SYSTÈME QtTest : pilote la vraie fenêtre (saisie, clics) contre le serveur
// désigné par P4_URL. Sans P4_URL, le programme renvoie 77 : CTest le marque « ignoré ».
//   Windows : $env:P4_URL = "http://127.0.0.1:8080"; ctest --preset windows-mingw-qt -L qt-systeme
// Option : P4_CAPTURE=<dossier> enregistre une capture de la fenêtre en fin de partie.
#include "ClientApi.hpp"
#include "FenetrePrincipale.hpp"

#include <QDir>
#include <QtTest>

#include <cstdio>

class TestFenetre : public QObject {
    Q_OBJECT

private:
    // Clique sur une colonne et attend la réponse du serveur (attente sur un état observable)
    static void jouer(FenetrePrincipale& fenetre, int colonne) {
        const int avant = fenetre.partie()->nombreDeCoups;
        auto* bouton = fenetre.findChild<QPushButton*>(QStringLiteral("colonne-%1").arg(colonne));
        QVERIFY(bouton != nullptr);
        QVERIFY(bouton->isEnabled());
        QTest::mouseClick(bouton, Qt::LeftButton);
        QTRY_COMPARE_WITH_TIMEOUT(fenetre.partie()->nombreDeCoups, avant + 1, 5000);
    }

private slots:
    void une_partie_jouee_jusqu_a_la_victoire_desactive_les_colonnes() {
        ClientApi client{QUrl(qEnvironmentVariable("P4_URL"))};
        FenetrePrincipale fenetre(client);
        fenetre.show();
        QVERIFY(QTest::qWaitForWindowExposed(&fenetre));

        // Saisie comme un utilisateur
        auto* joueur1 = fenetre.findChild<QLineEdit*>("joueur1");
        joueur1->clear();
        QTest::keyClicks(joueur1, "Chloe");  // keyClicks ne simule que des touches ASCII
        QSignalSpy partieRecue(&client, &ClientApi::partieRecue);
        QTest::mouseClick(fenetre.findChild<QPushButton*>("nouvellePartie"), Qt::LeftButton);
        QVERIFY(partieRecue.wait(5000));
        QCOMPARE(fenetre.findChild<QLabel*>("etat")->text(), QStringLiteral("Au tour de Chloe"));

        for (int colonne : {0, 1, 0, 1, 0, 1, 0}) {
            jouer(fenetre, colonne);
        }

        QCOMPARE(fenetre.findChild<QLabel*>("etat")->text(), QStringLiteral("Partie terminée — victoire de Chloe"));
        QCOMPARE(fenetre.findChild<QLabel*>("case-2-0")->property("jeton").toString(), QStringLiteral("R"));
        for (int colonne = 0; colonne < 7; ++colonne) {
            QVERIFY(!fenetre.findChild<QPushButton*>(QStringLiteral("colonne-%1").arg(colonne))->isEnabled());
        }
        if (const QString dossier = qEnvironmentVariable("P4_CAPTURE"); !dossier.isEmpty()) {
            fenetre.grab().save(QDir(dossier).filePath("client-qt-victoire.png"));
        }
    }

    void une_erreur_du_serveur_est_affichee() {
        ClientApi client{QUrl(qEnvironmentVariable("P4_URL"))};
        FenetrePrincipale fenetre(client);
        fenetre.show();
        fenetre.findChild<QLineEdit*>("joueur2")->setText(QStringLiteral("alice"));  // même nom (EX-05)
        QSignalSpy erreur(&client, &ClientApi::erreur);

        QTest::mouseClick(fenetre.findChild<QPushButton*>("nouvellePartie"), Qt::LeftButton);

        QVERIFY(erreur.wait(5000));
        QCOMPARE(fenetre.findChild<QLabel*>("message")->text(),
                 QStringLiteral("Les deux joueurs doivent avoir des noms différents"));
    }
};

int main(int argc, char* argv[]) {
    if (qEnvironmentVariableIsEmpty("P4_URL")) {
        std::puts("P4_URL non defini : aucun serveur a tester, test ignore");
        return 77;  // code « ignoré » convenu avec CTest (SKIP_RETURN_CODE)
    }
    QApplication application(argc, argv);
    TestFenetre test;
    return QTest::qExec(&test, argc, argv);
}
#include "tst_fenetre.moc"
