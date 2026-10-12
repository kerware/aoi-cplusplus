// Dernière génération/modification faite par l'IA Claude le 09/10/2026 17:45:07
// Client Qt : puissance4_qt [url du serveur]   (défaut : P4_URL, sinon http://127.0.0.1:8080)
#include "ClientApi.hpp"
#include "FenetrePrincipale.hpp"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication application(argc, argv);
    QString url = qEnvironmentVariable("P4_URL", QStringLiteral("http://127.0.0.1:8080"));
    if (argc > 1) {
        url = QString::fromLocal8Bit(argv[1]);
    }
    ClientApi client{QUrl(url)};
    FenetrePrincipale fenetre(client);
    fenetre.show();
    return QApplication::exec();
}
