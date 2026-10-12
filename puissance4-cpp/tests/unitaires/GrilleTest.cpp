// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#include "domaine/Grille.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <utility>

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
