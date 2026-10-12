// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
// Test unitaire du cas d'utilisation : les ports de sortie sont remplacés
// par des bouchons GoogleMock. Aucun adaptateur réel n'est utilisé.
#include "application/ServicePartie.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <stdexcept>

using namespace p4;
using ::testing::_;
using ::testing::Property;
using ::testing::Return;
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

TEST_F(ServicePartieTest, une_demande_invalide_ne_sauvegarde_rien) {
    EXPECT_CALL(generateur, suivant()).WillOnce(Return("P0001"));
    EXPECT_CALL(repository, sauvegarder(_)).Times(0);

    EXPECT_THROW(service.creer({"", "Bob"}), std::invalid_argument);
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
