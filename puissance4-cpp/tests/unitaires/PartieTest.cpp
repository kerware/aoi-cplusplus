// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#include "domaine/Partie.hpp"

#include <gtest/gtest.h>

#include <stdexcept>

using p4::domaine::Jeton;
using p4::domaine::Partie;
using p4::domaine::StatutPartie;

// Fixture : une partie Alice contre Bob, recréée avant chaque test
class PartieTest : public ::testing::Test {
protected:
    Partie partie{"P0001", "Alice", "Bob"};
};

TEST_F(PartieTest, une_nouvelle_partie_est_en_cours) {
    EXPECT_EQ(partie.statut(), StatutPartie::EnCours);
    EXPECT_EQ(partie.id(), "P0001");
}

TEST_F(PartieTest, le_joueur_1_a_les_rouges_et_le_joueur_2_les_jaunes) {
    EXPECT_EQ(partie.joueur1().nom(), "Alice");
    EXPECT_EQ(partie.joueur1().couleur(), Jeton::Rouge);
    EXPECT_EQ(partie.joueur2().nom(), "Bob");
    EXPECT_EQ(partie.joueur2().couleur(), Jeton::Jaune);
}

TEST_F(PartieTest, le_joueur_1_commence) {
    EXPECT_EQ(partie.joueurCourant(), partie.joueur1());
}

TEST_F(PartieTest, la_grille_d_une_nouvelle_partie_est_vide) {
    EXPECT_TRUE(partie.grille().estVide());
}

TEST(PartieCreationTest, l_identifiant_est_obligatoire) {
    EXPECT_THROW(Partie("", "Alice", "Bob"), std::invalid_argument);
}

TEST(PartieCreationTest, un_nom_de_joueur_invalide_empeche_la_creation) {
    EXPECT_THROW(Partie("P0001", "", "Bob"), std::invalid_argument);
    EXPECT_THROW(Partie("P0001", "Alice", " "), std::invalid_argument);
}
