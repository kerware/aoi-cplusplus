// Dernière génération/modification faite par l'IA Claude le 13/10/2026 19:31:19
// Test d'intégration : le VRAI serveur HTTP est démarré sur un port libre,
// branché sur le vrai service et le vrai repository en mémoire.
// Un client HTTP (cpp-httplib) l'interroge comme le ferait l'IHM.
#include "adaptateurs/entree/rest/ServeurRest.hpp"
#include "adaptateurs/sortie/memoire/GenerateurIdentifiantSequentiel.hpp"
#include "adaptateurs/sortie/memoire/PartieRepositoryEnMemoire.hpp"
#include "application/ServicePartie.hpp"

#include <gtest/gtest.h>
#include <httplib.h>
#include <nlohmann/json.hpp>

#include <chrono>
#include <memory>
#include <thread>

using namespace p4;
using nlohmann::json;

namespace {

constexpr const char* TYPE_JSON = "application/json";

class ServeurRestTest : public ::testing::Test {
protected:
    void SetUp() override {
        port = serveur.lier("127.0.0.1", 0);
        ASSERT_GT(port, 0);
        ecoute = std::thread([this] { serveur.ecouter(); });
        for (int essai = 0; essai < 100 && !serveur.estDemarre(); ++essai) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        ASSERT_TRUE(serveur.estDemarre());
        client = std::make_unique<httplib::Client>("127.0.0.1", port);
    }

    void TearDown() override {
        serveur.arreter();
        if (ecoute.joinable()) {
            ecoute.join();
        }
    }

    adaptateurs::PartieRepositoryEnMemoire repository;
    adaptateurs::GenerateurIdentifiantSequentiel generateur;
    application::ServicePartie service{repository, generateur};
    adaptateurs::ServeurRest serveur{service, service, service, P4_DOSSIER_WEB, {"9.9.9", "abc1234", "42"}};
    std::thread ecoute;
    int port = 0;
    std::unique_ptr<httplib::Client> client;
};

} // namespace

TEST_F(ServeurRestTest, le_point_de_sante_repond_ok) {
    const auto reponse = client->Get("/api/sante");

    ASSERT_TRUE(reponse);
    EXPECT_EQ(reponse->status, 200);
    EXPECT_EQ(json::parse(reponse->body)["statut"], "OK");
}

// EX-21 : le point de santé identifie le binaire déployé
TEST_F(ServeurRestTest, le_point_de_sante_indique_version_commit_et_build) {
    const json sante = json::parse(client->Get("/api/sante")->body);

    EXPECT_EQ(sante["version"], "9.9.9");
    EXPECT_EQ(sante["commit"], "abc1234");
    EXPECT_EQ(sante["build"], "42");
}

TEST_F(ServeurRestTest, post_parties_cree_une_partie_et_repond_201) {
    const auto reponse = client->Post("/api/parties", R"({"joueur1":"Alice","joueur2":"Bob"})", TYPE_JSON);

    ASSERT_TRUE(reponse);
    EXPECT_EQ(reponse->status, 201);
    EXPECT_EQ(reponse->get_header_value("Location"), "/api/parties/P0001");
    const json partie = json::parse(reponse->body);
    EXPECT_EQ(partie["id"], "P0001");
    EXPECT_EQ(partie["statut"], "EN_COURS");
    EXPECT_EQ(partie["joueur1"]["couleur"], "ROUGE");
    EXPECT_EQ(partie["joueurCourant"], "Alice");
    ASSERT_EQ(partie["grille"].size(), 6u);
    EXPECT_EQ(partie["grille"][0], ".......");
    EXPECT_EQ(repository.nombreDeParties(), 1u);
}

TEST_F(ServeurRestTest, une_partie_creee_peut_etre_consultee) {
    client->Post("/api/parties", R"({"joueur1":"Alice","joueur2":"Bob"})", TYPE_JSON);

    const auto reponse = client->Get("/api/parties/P0001");

    ASSERT_TRUE(reponse);
    EXPECT_EQ(reponse->status, 200);
    EXPECT_EQ(json::parse(reponse->body)["joueur2"]["nom"], "Bob");
}

TEST_F(ServeurRestTest, consulter_une_partie_inconnue_repond_404) {
    const auto reponse = client->Get("/api/parties/P0404");

    ASSERT_TRUE(reponse);
    EXPECT_EQ(reponse->status, 404);
    EXPECT_TRUE(json::parse(reponse->body).contains("erreur"));
}

TEST_F(ServeurRestTest, un_corps_qui_n_est_pas_du_json_repond_400) {
    const auto reponse = client->Post("/api/parties", "pas du json", TYPE_JSON);

    ASSERT_TRUE(reponse);
    EXPECT_EQ(reponse->status, 400);
}

TEST_F(ServeurRestTest, un_nom_de_joueur_vide_repond_400_sans_creer_de_partie) {
    const auto reponse = client->Post("/api/parties", R"({"joueur1":"","joueur2":"Bob"})", TYPE_JSON);

    ASSERT_TRUE(reponse);
    EXPECT_EQ(reponse->status, 400);
    EXPECT_EQ(json::parse(reponse->body)["erreur"], "Le nom du joueur est obligatoire");
    EXPECT_EQ(repository.nombreDeParties(), 0u);
}

// [TP1 - ex. 6]
TEST_F(ServeurRestTest, deux_joueurs_de_meme_nom_repond_400) {
    const auto reponse = client->Post("/api/parties", R"({"joueur1":"Alice","joueur2":"ALICE"})", TYPE_JSON);

    ASSERT_TRUE(reponse);
    EXPECT_EQ(reponse->status, 400);
    EXPECT_EQ(json::parse(reponse->body)["erreur"], "Les deux joueurs doivent avoir des noms différents");
    EXPECT_EQ(repository.nombreDeParties(), 0u);
}

// [TP1 - ex. 5] Reproduction automatisée de l'anomalie observée dans l'IHM
TEST_F(ServeurRestTest, une_creation_refusee_ne_consomme_pas_de_numero_de_partie) {
    client->Post("/api/parties", R"({"joueur1":"","joueur2":"Bob"})", TYPE_JSON);

    const auto reponse = client->Post("/api/parties", R"({"joueur1":"Alice","joueur2":"Bob"})", TYPE_JSON);

    ASSERT_TRUE(reponse);
    EXPECT_EQ(reponse->get_header_value("Location"), "/api/parties/P0001");
}

TEST_F(ServeurRestTest, un_nom_de_type_incorrect_repond_400) {
    const auto reponse = client->Post("/api/parties", R"({"joueur1":42,"joueur2":"Bob"})", TYPE_JSON);

    ASSERT_TRUE(reponse);
    EXPECT_EQ(reponse->status, 400);
}

TEST_F(ServeurRestTest, la_racine_sert_l_ihm_html5) {
    const auto reponse = client->Get("/");

    ASSERT_TRUE(reponse);
    EXPECT_EQ(reponse->status, 200);
    EXPECT_NE(reponse->body.find("Puissance 4"), std::string::npos);
}

// --- Itération 2 : POST /api/parties/{id}/coups --------------------------------

namespace {
constexpr const char* NOUVELLE_PARTIE = R"({"joueur1":"Alice","joueur2":"Bob"})";
}

TEST_F(ServeurRestTest, jouer_un_coup_repond_200_avec_la_grille_a_jour) {
    client->Post("/api/parties", NOUVELLE_PARTIE, TYPE_JSON);

    const auto reponse = client->Post("/api/parties/P0001/coups", R"({"colonne":3})", TYPE_JSON);

    ASSERT_TRUE(reponse);
    EXPECT_EQ(reponse->status, 200);
    const json partie = json::parse(reponse->body);
    EXPECT_EQ(partie["grille"][5], "...R...");
    EXPECT_EQ(partie["joueurCourant"], "Bob");
    EXPECT_EQ(partie["nombreDeCoups"], 1);
    EXPECT_EQ(partie["dernierCoup"]["ligne"], 5);
}

TEST_F(ServeurRestTest, jouer_dans_une_partie_inconnue_repond_404) {
    const auto reponse = client->Post("/api/parties/P0404/coups", R"({"colonne":3})", TYPE_JSON);

    ASSERT_TRUE(reponse);
    EXPECT_EQ(reponse->status, 404);
}

TEST_F(ServeurRestTest, jouer_dans_une_colonne_pleine_repond_409) {
    client->Post("/api/parties", NOUVELLE_PARTIE, TYPE_JSON);
    for (int i = 0; i < 6; ++i) {
        client->Post("/api/parties/P0001/coups", R"({"colonne":0})", TYPE_JSON);
    }

    const auto reponse = client->Post("/api/parties/P0001/coups", R"({"colonne":0})", TYPE_JSON);

    ASSERT_TRUE(reponse);
    EXPECT_EQ(reponse->status, 409);
    EXPECT_EQ(json::parse(client->Get("/api/parties/P0001")->body)["nombreDeCoups"], 6);
}

TEST_F(ServeurRestTest, jouer_sans_colonne_repond_400) {
    client->Post("/api/parties", NOUVELLE_PARTIE, TYPE_JSON);

    const auto reponse = client->Post("/api/parties/P0001/coups", R"({})", TYPE_JSON);

    ASSERT_TRUE(reponse);
    EXPECT_EQ(reponse->status, 400);
}

// --- [TP2] Tests issus de la conception (partitions EX-08) --------------------

TEST_F(ServeurRestTest, CT_08_03_jouer_en_colonne_7_repond_400) {
    client->Post("/api/parties", NOUVELLE_PARTIE, TYPE_JSON);

    const auto reponse = client->Post("/api/parties/P0001/coups", R"({"colonne":7})", TYPE_JSON);

    ASSERT_TRUE(reponse);
    EXPECT_EQ(reponse->status, 400);
}

class ServeurRestColonnesNonEntieresTest : public ServeurRestTest,
                                           public ::testing::WithParamInterface<const char*> {};

TEST_P(ServeurRestColonnesNonEntieresTest, CT_08_04_une_colonne_non_entiere_repond_400_sans_jouer) {
    client->Post("/api/parties", NOUVELLE_PARTIE, TYPE_JSON);

    const auto reponse = client->Post("/api/parties/P0001/coups", GetParam(), TYPE_JSON);

    ASSERT_TRUE(reponse);
    EXPECT_EQ(reponse->status, 400);
    EXPECT_EQ(json::parse(client->Get("/api/parties/P0001")->body)["nombreDeCoups"], 0);
}

INSTANTIATE_TEST_SUITE_P(EX08, ServeurRestColonnesNonEntieresTest,
                         ::testing::Values(R"({"colonne":2.9})", R"({"colonne":"3"})", R"({"colonne":1e20})",
                                           R"({"colonne":9999999999})", R"({"colonne":null})"));

// --- Itération 3 : la victoire est visible dans l'API (EX-15) -------------------

TEST_F(ServeurRestTest, une_victoire_horizontale_est_renvoyee_par_l_api) {
    client->Post("/api/parties", NOUVELLE_PARTIE, TYPE_JSON);
    for (int colonne : {0, 0, 1, 1, 2, 2}) {
        client->Post("/api/parties/P0001/coups", json{{"colonne", colonne}}.dump(), TYPE_JSON);
    }

    const auto reponse = client->Post("/api/parties/P0001/coups", R"({"colonne":3})", TYPE_JSON);

    ASSERT_TRUE(reponse);
    EXPECT_EQ(reponse->status, 200);
    const json partie = json::parse(reponse->body);
    EXPECT_EQ(partie["statut"], "GAGNEE");
    EXPECT_EQ(partie["vainqueur"], "Alice");
}

// --- Fin de partie dans l'API (EX-15, EX-16, EX-17) -------------------

TEST_F(ServeurRestTest, un_coup_apres_la_victoire_repond_409) {
    client->Post("/api/parties", NOUVELLE_PARTIE, TYPE_JSON);
    for (int colonne : {0, 0, 1, 1, 2, 2, 3}) {
        client->Post("/api/parties/P0001/coups", json{{"colonne", colonne}}.dump(), TYPE_JSON);
    }

    const auto reponse = client->Post("/api/parties/P0001/coups", R"({"colonne":5})", TYPE_JSON);

    ASSERT_TRUE(reponse);
    EXPECT_EQ(reponse->status, 409);
    EXPECT_EQ(json::parse(reponse->body)["erreur"], "Partie terminée : plus aucun coup n'est accepté");
}

TEST_F(ServeurRestTest, une_grille_pleine_sans_alignement_donne_le_statut_NULLE) {
    client->Post("/api/parties", NOUVELLE_PARTIE, TYPE_JSON);
    for (int colonne : {5, 4, 5, 0, 6, 2, 4, 5, 5, 0, 4, 1, 1, 0, 4, 5, 6, 5, 3, 1, 1, 2, 2, 6, 2, 6, 6, 3, 6, 2, 0, 3, 0, 3, 3, 4, 3, 1, 4, 2, 1, 0}) {
        client->Post("/api/parties/P0001/coups", json{{"colonne", colonne}}.dump(), TYPE_JSON);
    }

    const json partie = json::parse(client->Get("/api/parties/P0001")->body);
    EXPECT_EQ(partie["statut"], "NULLE");
    EXPECT_TRUE(partie["vainqueur"].is_null());
}
