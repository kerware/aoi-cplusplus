// Dernière génération/modification faite par l'IA Claude le 09/10/2026 16:47:56
// Test unitaire du cas d'utilisation : les ports de sortie sont remplacés
// par des bouchons GoogleMock. Aucun adaptateur réel n'est utilisé.
#include "application/ServicePartie.hpp"
#include "application/Erreurs.hpp"
#include "domaine/Erreurs.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <stdexcept>

using namespace p4;
using ::testing::_;
using ::testing::Property;
using ::testing::Return;
using ::testing::InSequence;
using ::testing::StrictMock;

namespace {

class PartieRepositoryMock : public application::PartieRepository {
public:
    MOCK_METHOD(void, sauvegarder, (const domaine::Partie&), (override));
    MOCK_METHOD(std::optional<domaine::Partie>, trouverParId, (const domaine::IdPartie&), (const, override));
};

class GenerateurIdentifiantMock : public application::GenerateurIdentifiant {
public:
    MOCK_METHOD(domaine::IdPartie, suivant, (), (override));
};

class ServicePartieTest : public ::testing::Test {
protected:
    StrictMock<PartieRepositoryMock> repository;
    StrictMock<GenerateurIdentifiantMock> generateur;
    application::ServicePartie service{repository, generateur};
};

} // namespace

TEST_F(ServicePartieTest, creer_une_partie_lui_donne_l_identifiant_du_generateur) {
    EXPECT_CALL(generateur, suivant()).WillOnce(Return("P0042"));
    EXPECT_CALL(repository, sauvegarder(_));

    const domaine::Partie partie = service.creer({"Alice", "Bob"});

    EXPECT_EQ(partie.id(), "P0042");
    EXPECT_EQ(partie.joueur1().nom(), "Alice");
    EXPECT_EQ(partie.joueur2().nom(), "Bob");
}

TEST_F(ServicePartieTest, creer_une_partie_la_sauvegarde_dans_le_repository) {
    EXPECT_CALL(generateur, suivant()).WillOnce(Return("P0001"));
    EXPECT_CALL(repository, sauvegarder(Property(&domaine::Partie::id, "P0001"))).Times(1);

    service.creer({"Alice", "Bob"});
}

// [TP1 - ex. 5] Ce test existant attendait un appel au générateur (WillOnce) :
// avec la correction, le générateur n'est plus appelé et le test échouait.
// On retire cette attente devenue fausse — un test qui casse après une
// correction est un signal à analyser, pas à faire taire.
TEST_F(ServicePartieTest, une_demande_invalide_ne_sauvegarde_rien) {
    EXPECT_CALL(repository, sauvegarder(_)).Times(0);

    EXPECT_THROW(service.creer({"", "Bob"}), std::invalid_argument);
}

// [TP1 - ex. 5] Test qui révèle le défaut "numéro consommé" (rouge avant correction)
TEST_F(ServicePartieTest, une_demande_invalide_ne_consomme_pas_d_identifiant) {
    EXPECT_CALL(generateur, suivant()).Times(0);

    EXPECT_THROW(service.creer({"", "Bob"}), std::invalid_argument);
}

// [TP1 - ex. 6]
TEST_F(ServicePartieTest, deux_noms_identiques_sont_refuses_sans_rien_consommer) {
    EXPECT_CALL(generateur, suivant()).Times(0);
    EXPECT_CALL(repository, sauvegarder(_)).Times(0);

    EXPECT_THROW(service.creer({"Alice", "alice"}), std::invalid_argument);
}

TEST_F(ServicePartieTest, consulter_une_partie_existante_la_retourne) {
    EXPECT_CALL(repository, trouverParId("P0001"))
        .WillOnce(Return(domaine::Partie("P0001", "Alice", "Bob")));

    const auto partie = service.consulter("P0001");

    ASSERT_TRUE(partie.has_value());
    EXPECT_EQ(partie->joueur1().nom(), "Alice");
}

TEST_F(ServicePartieTest, consulter_une_partie_inconnue_ne_retourne_rien) {
    EXPECT_CALL(repository, trouverParId("P9999")).WillOnce(Return(std::nullopt));

    EXPECT_FALSE(service.consulter("P9999").has_value());
}

// --- Itération 2 : jouer un coup ---------------------------------------------

TEST_F(ServicePartieTest, jouer_un_coup_sauvegarde_la_partie_a_jour) {
    EXPECT_CALL(repository, trouverParId("P0001"))
        .WillOnce(Return(domaine::Partie("P0001", "Alice", "Bob")));
    EXPECT_CALL(repository, sauvegarder(Property(&domaine::Partie::nombreDeCoups, 1)));

    const domaine::Partie partie = service.jouer("P0001", 3);

    EXPECT_EQ(partie.joueurCourant().nom(), "Bob");
}

TEST_F(ServicePartieTest, jouer_dans_une_partie_inconnue_leve_PartieInconnue) {
    EXPECT_CALL(repository, trouverParId("P0404")).WillOnce(Return(std::nullopt));
    EXPECT_CALL(repository, sauvegarder(_)).Times(0);

    EXPECT_THROW(service.jouer("P0404", 3), application::PartieInconnue);
}

// --- GoogleMock : ordre des appels et partie terminée --------------------

TEST_F(ServicePartieTest, jouer_lit_la_partie_puis_la_sauvegarde_dans_cet_ordre) {
    InSequence ordre;  // les attentes suivantes doivent être satisfaites dans l'ordre
    EXPECT_CALL(repository, trouverParId("P0001"))
        .WillOnce(Return(domaine::Partie("P0001", "Alice", "Bob")));
    EXPECT_CALL(repository, sauvegarder(_));

    service.jouer("P0001", 0);
}

TEST_F(ServicePartieTest, jouer_dans_une_partie_terminee_ne_sauvegarde_rien) {
    domaine::Partie gagnee("P0001", "Alice", "Bob");
    for (int i = 0; i < 3; ++i) {
        gagnee.jouer(0);
        gagnee.jouer(1);
    }
    gagnee.jouer(0);
    EXPECT_CALL(repository, trouverParId("P0001")).WillOnce(Return(gagnee));
    EXPECT_CALL(repository, sauvegarder(_)).Times(0);

    EXPECT_THROW(service.jouer("P0001", 3), domaine::PartieTerminee);
}

// Un coup refusé (colonne pleine) ne sauvegarde rien
TEST_F(ServicePartieTest, un_coup_refuse_ne_sauvegarde_rien) {
    domaine::Partie partie("P0001", "Alice", "Bob");
    for (int i = 0; i < domaine::Grille::LIGNES; ++i) {
        partie.jouer(0);
    }
    EXPECT_CALL(repository, trouverParId("P0001")).WillOnce(Return(partie));
    EXPECT_CALL(repository, sauvegarder(_)).Times(0);

    EXPECT_THROW(service.jouer("P0001", 0), domaine::ColonnePleine);
}
