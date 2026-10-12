// Dernière génération/modification faite par l'IA Claude le 09/10/2026 15:10:00
#include "domaine/Grille.hpp"
#include "domaine/Erreurs.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <utility>

using p4::domaine::ColonnePleine;
using p4::domaine::CoupInvalide;
using p4::domaine::Grille;
using p4::domaine::Jeton;

TEST(GrilleTest, une_grille_neuve_fait_6_lignes_et_7_colonnes) {
    EXPECT_EQ(Grille::LIGNES, 6);
    EXPECT_EQ(Grille::COLONNES, 7);
}

TEST(GrilleTest, une_grille_neuve_est_vide) {
    const Grille grille;

    EXPECT_TRUE(grille.estVide());
    for (int ligne = 0; ligne < Grille::LIGNES; ++ligne) {
        for (int colonne = 0; colonne < Grille::COLONNES; ++colonne) {
            EXPECT_EQ(grille.caseEn(ligne, colonne), Jeton::Vide) << "case (" << ligne << "," << colonne << ")";
        }
    }
}

// Test paramétré : analyse des valeurs limites sur les coordonnées
class GrilleHorsLimitesTest : public ::testing::TestWithParam<std::pair<int, int>> {};

TEST_P(GrilleHorsLimitesTest, lire_une_case_hors_grille_leve_out_of_range) {
    const Grille grille;
    const auto [ligne, colonne] = GetParam();

    EXPECT_THROW((void)grille.caseEn(ligne, colonne), std::out_of_range);
}

INSTANTIATE_TEST_SUITE_P(ValeursLimites, GrilleHorsLimitesTest,
                         ::testing::Values(std::pair{-1, 0}, std::pair{6, 0},
                                           std::pair{0, -1}, std::pair{0, 7}));

TEST(GrilleTest, les_coins_de_la_grille_sont_accessibles) {
    const Grille grille;

    EXPECT_NO_THROW((void)grille.caseEn(0, 0));
    EXPECT_NO_THROW((void)grille.caseEn(5, 6));
}

// --- Itération 2 : jouer un coup (EX-06, EX-08, EX-09) ----------------------

TEST(GrilleJouerTest, un_jeton_joue_dans_une_colonne_vide_tombe_en_bas) {
    Grille grille;

    const int ligne = grille.jouer(3, Jeton::Rouge);

    EXPECT_EQ(ligne, 5);
    EXPECT_EQ(grille.caseEn(5, 3), Jeton::Rouge);
    EXPECT_EQ(grille.nombreDeJetons(), 1);
}

TEST(GrilleJouerTest, les_jetons_s_empilent_dans_une_colonne) {
    Grille grille;
    grille.jouer(0, Jeton::Rouge);

    const int ligne = grille.jouer(0, Jeton::Jaune);

    EXPECT_EQ(ligne, 4);
    EXPECT_EQ(grille.caseEn(4, 0), Jeton::Jaune);
}

TEST(GrilleJouerTest, jouer_dans_une_colonne_pleine_leve_ColonnePleine) {
    Grille grille;
    for (int i = 0; i < Grille::LIGNES; ++i) {
        grille.jouer(6, i % 2 ? Jeton::Jaune : Jeton::Rouge);
    }

    EXPECT_TRUE(grille.colonnePleine(6));
    EXPECT_THROW(grille.jouer(6, Jeton::Rouge), ColonnePleine);
    EXPECT_EQ(grille.nombreDeJetons(), 6);
}

TEST(GrilleJouerTest, jouer_dans_une_colonne_negative_leve_CoupInvalide) {
    Grille grille;

    EXPECT_THROW(grille.jouer(-1, Jeton::Rouge), CoupInvalide);
    EXPECT_TRUE(grille.estVide());
}
