// Dernière génération/modification faite par l'IA Claude le 09/10/2026 15:54:22
#include "domaine/Partie.hpp"
#include "domaine/Erreurs.hpp"

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

// --- [TP1 - ex. 6] Règle : les deux joueurs ont des noms différents ---------

TEST(PartieCreationTest, deux_joueurs_ne_peuvent_pas_avoir_le_meme_nom) {
    EXPECT_THROW(Partie("P0001", "Alice", "Alice"), std::invalid_argument);
}

TEST(PartieCreationTest, la_comparaison_des_noms_ignore_la_casse_et_les_espaces_de_bord) {
    EXPECT_THROW(Partie("P0001", "Alice", "  aLiCe "), std::invalid_argument);
}

TEST(PartieCreationTest, des_noms_proches_mais_differents_sont_acceptes) {
    EXPECT_NO_THROW(Partie("P0001", "Alice", "Alicia"));
}

// --- Itération 2 : jouer un coup (EX-06, EX-07, EX-09, EX-11) ---------------

TEST_F(PartieTest, apres_le_coup_du_joueur_1_la_main_passe_au_joueur_2) {
    partie.jouer(3);

    EXPECT_EQ(partie.joueurCourant(), partie.joueur2());
    EXPECT_EQ(partie.nombreDeCoups(), 1);
}

TEST_F(PartieTest, les_joueurs_alternent) {
    partie.jouer(3);
    partie.jouer(3);
    partie.jouer(4);

    EXPECT_EQ(partie.joueurCourant(), partie.joueur2());
    EXPECT_EQ(partie.grille().caseEn(5, 3), Jeton::Rouge);
    EXPECT_EQ(partie.grille().caseEn(4, 3), Jeton::Jaune);
    EXPECT_EQ(partie.grille().caseEn(5, 4), Jeton::Rouge);
}

TEST_F(PartieTest, le_dernier_coup_est_memorise) {
    partie.jouer(2);

    ASSERT_TRUE(partie.dernierCoup().has_value());
    EXPECT_EQ(partie.dernierCoup()->colonne, 2);
    EXPECT_EQ(partie.dernierCoup()->ligne, 5);
    EXPECT_EQ(partie.dernierCoup()->couleur, Jeton::Rouge);
    EXPECT_EQ(partie.dernierCoup()->joueur, "Alice");
}

TEST_F(PartieTest, un_coup_dans_une_colonne_pleine_ne_change_pas_la_main) {
    for (int i = 0; i < p4::domaine::Grille::LIGNES; ++i) {
        partie.jouer(0);
    }
    const auto courantAvant = partie.joueurCourant();

    EXPECT_THROW(partie.jouer(0), p4::domaine::ColonnePleine);
    EXPECT_EQ(partie.joueurCourant(), courantAvant);
    EXPECT_EQ(partie.nombreDeCoups(), 6);
}

// --- Itération 3 (TDD) : victoire horizontale (EX-14, EX-15) ------------------

TEST_F(PartieTest, quatre_jetons_alignes_horizontalement_font_gagner_le_joueur) {
    for (int colonne = 0; colonne < 3; ++colonne) {
        partie.jouer(colonne);  // Alice, ligne du bas
        partie.jouer(colonne);  // Bob, au-dessus
    }
    EXPECT_EQ(partie.statut(), StatutPartie::EnCours);

    partie.jouer(3);  // Alice aligne 4 jetons

    EXPECT_EQ(partie.statut(), StatutPartie::Gagnee);
    ASSERT_TRUE(partie.vainqueur().has_value());
    EXPECT_EQ(partie.vainqueur()->nom(), "Alice");
}

TEST_F(PartieTest, une_partie_neuve_n_a_pas_de_vainqueur) {
    EXPECT_FALSE(partie.vainqueur().has_value());
}
