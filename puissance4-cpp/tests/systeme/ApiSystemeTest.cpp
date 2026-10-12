// Dernière génération/modification faite par l'IA Claude le 09/10/2026 16:58:00
// ---------------------------------------------------------------------------
// Tests SYSTÈME de l'API : ils s'exécutent contre un serveur DÉJÀ DÉPLOYÉ
// (exécutable Windows, conteneur Docker, serveur de recette…), désigné par P4_URL.
//   PowerShell : $env:P4_URL = "http://127.0.0.1:8080"; ctest --preset windows-msvc -L systeme
//   Linux      : P4_URL=http://serveur:8080 ctest --preset linux-gcc -L systeme
// Sans P4_URL, les tests sont ignorés (SKIPPED) : ils ne bloquent pas la boucle TDD.
//
// Bonnes pratiques illustrées :
//   - aucun accès au code interne : seulement HTTP + JSON, comme un client réel ;
//   - chaque test crée SA partie et utilise l'identifiant renvoyé (jamais « P0001 ») :
//     les tests sont indépendants de l'état du serveur et peuvent tourner en parallèle.
// ---------------------------------------------------------------------------
#include <gtest/gtest.h>
#include <httplib.h>
#include <nlohmann/json.hpp>

#include <cstdlib>
#include <memory>
#include <string>

using nlohmann::json;

namespace {

constexpr const char* TYPE_JSON = "application/json";

class ApiSystemeTest : public ::testing::Test {
protected:
    void SetUp() override {
        const char* url = std::getenv("P4_URL");
        if (url == nullptr || std::string(url).empty()) {
            GTEST_SKIP() << "P4_URL non défini : aucun serveur à tester";
        }
        client = std::make_unique<httplib::Client>(url);
        client->set_connection_timeout(5);
        client->set_read_timeout(5);
        const auto sante = client->Get("/api/sante");
        ASSERT_TRUE(sante) << "serveur injoignable : " << url;
        ASSERT_EQ(sante->status, 200);
    }

    // Crée une partie et renvoie son identifiant (lu dans la réponse, pas supposé).
    std::string nouvellePartie(const std::string& joueur1 = "Alice", const std::string& joueur2 = "Bob") {
        const auto reponse =
            client->Post("/api/parties", json{{"joueur1", joueur1}, {"joueur2", joueur2}}.dump(), TYPE_JSON);
        EXPECT_TRUE(reponse);
        EXPECT_EQ(reponse->status, 201);
        return json::parse(reponse->body)["id"].get<std::string>();
    }

    httplib::Result jouer(const std::string& id, int colonne) {
        return client->Post("/api/parties/" + id + "/coups", json{{"colonne", colonne}}.dump(), TYPE_JSON);
    }

    std::unique_ptr<httplib::Client> client;
};

} // namespace

TEST_F(ApiSystemeTest, le_point_de_sante_indique_la_version) {
    const json sante = json::parse(client->Get("/api/sante")->body);

    EXPECT_EQ(sante["statut"], "OK");
    EXPECT_TRUE(sante.contains("version"));
}

TEST_F(ApiSystemeTest, parcours_complet_jusqu_a_une_victoire_verticale) {
    const std::string id = nouvellePartie();

    for (int i = 0; i < 3; ++i) {
        ASSERT_EQ(jouer(id, 0)->status, 200);  // Alice
        ASSERT_EQ(jouer(id, 1)->status, 200);  // Bob
    }
    const auto victoire = jouer(id, 0);

    ASSERT_TRUE(victoire);
    EXPECT_EQ(victoire->status, 200);
    const json partie = json::parse(victoire->body);
    EXPECT_EQ(partie["statut"], "GAGNEE");
    EXPECT_EQ(partie["vainqueur"], "Alice");
    EXPECT_EQ(partie["nombreDeCoups"], 7);
}

TEST_F(ApiSystemeTest, aucun_coup_n_est_accepte_apres_la_fin_de_partie) {
    const std::string id = nouvellePartie();
    for (int colonne : {0, 1, 0, 1, 0, 1, 0}) {
        jouer(id, colonne);
    }

    const auto reponse = jouer(id, 3);

    ASSERT_TRUE(reponse);
    EXPECT_EQ(reponse->status, 409);
}

TEST_F(ApiSystemeTest, deux_parties_simultanees_sont_independantes) {
    const std::string a = nouvellePartie("Alice", "Bob");
    const std::string b = nouvellePartie("Chloé", "David");

    jouer(a, 3);

    EXPECT_EQ(json::parse(client->Get("/api/parties/" + a)->body)["nombreDeCoups"], 1);
    EXPECT_EQ(json::parse(client->Get("/api/parties/" + b)->body)["nombreDeCoups"], 0);
}
