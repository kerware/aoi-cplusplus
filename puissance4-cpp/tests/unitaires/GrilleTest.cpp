// Dernière génération/modification faite par l'IA Claude le 09/10/2026 16:47:56
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

// --- [TP2] Tests issus de la conception (valeurs limites EX-08) ---------------
// Nommés d'après les cas de test Squash TM : CT-08-01 et CT-08-03.

class GrilleColonnesValidesTest : public ::testing::TestWithParam<int> {};

TEST_P(GrilleColonnesValidesTest, CT_08_01_une_colonne_limite_valide_est_acceptee) {
    Grille grille;

    EXPECT_EQ(grille.jouer(GetParam(), Jeton::Rouge), 5);
}

INSTANTIATE_TEST_SUITE_P(EX08, GrilleColonnesValidesTest, ::testing::Values(0, 6));

class GrilleColonnesInvalidesTest : public ::testing::TestWithParam<int> {};

TEST_P(GrilleColonnesInvalidesTest, CT_08_03_une_colonne_hors_grille_leve_CoupInvalide) {
    Grille grille;

    EXPECT_THROW(grille.jouer(GetParam(), Jeton::Rouge), CoupInvalide);
    EXPECT_TRUE(grille.estVide());
}

INSTANTIATE_TEST_SUITE_P(EX08, GrilleColonnesInvalidesTest, ::testing::Values(-1, 7, 100));

// --- Itération 3 (TDD) : alignement de 4 jetons — horizontal (EX-14) ---------

TEST(GrilleVictoireTest, quatre_jetons_alignes_horizontalement_forment_un_alignement) {
    Grille grille;
    for (int colonne = 0; colonne < 4; ++colonne) {
        grille.jouer(colonne, Jeton::Rouge);
    }

    EXPECT_TRUE(grille.aligneQuatre(5, 3));
}

TEST(GrilleVictoireTest, trois_jetons_alignes_ne_suffisent_pas) {
    Grille grille;
    for (int colonne = 0; colonne < 3; ++colonne) {
        grille.jouer(colonne, Jeton::Rouge);
    }

    EXPECT_FALSE(grille.aligneQuatre(5, 2));
}

TEST(GrilleVictoireTest, un_jeton_adverse_interrompt_l_alignement) {
    Grille grille;
    grille.jouer(0, Jeton::Rouge);
    grille.jouer(1, Jeton::Rouge);
    grille.jouer(2, Jeton::Jaune);
    grille.jouer(3, Jeton::Rouge);
    grille.jouer(4, Jeton::Rouge);

    EXPECT_FALSE(grille.aligneQuatre(5, 4));
}

TEST(GrilleVictoireTest, l_alignement_est_detecte_quand_le_dernier_jeton_comble_un_trou) {
    Grille grille;
    grille.jouer(0, Jeton::Jaune);
    grille.jouer(1, Jeton::Jaune);
    grille.jouer(3, Jeton::Jaune);
    grille.jouer(2, Jeton::Jaune);  // le dernier jeton est au milieu

    EXPECT_TRUE(grille.aligneQuatre(5, 2));
}

TEST(GrilleVictoireTest, une_case_vide_n_est_jamais_alignee) {
    const Grille grille;

    EXPECT_FALSE(grille.aligneQuatre(5, 0));
}

// --- TDD : verticale et diagonales (EX-14) -----------------

TEST(GrilleVictoireTest, quatre_jetons_empiles_forment_un_alignement_vertical) {
    Grille grille;
    for (int i = 0; i < 4; ++i) {
        grille.jouer(2, Jeton::Jaune);
    }

    EXPECT_TRUE(grille.aligneQuatre(2, 2));  // le 4e jeton est en ligne 2
}

TEST(GrilleVictoireTest, trois_jetons_empiles_ne_suffisent_pas) {
    Grille grille;
    for (int i = 0; i < 3; ++i) {
        grille.jouer(2, Jeton::Jaune);
    }

    EXPECT_FALSE(grille.aligneQuatre(3, 2));
}

// Une diagonale est construite colonne par colonne : on "cale" chaque jeton
// avec des jetons adverses pour atteindre la bonne hauteur.
struct Diagonale {
    const char* nom;
    int colonnes[4];   // colonne du jeton gagnant à chaque étage
    int cales[4];      // nombre de jetons adverses à poser d'abord dans cette colonne
};

// Affichage lisible du paramètre (sinon GoogleTest et CTest affichent les octets bruts)
std::ostream& operator<<(std::ostream& flux, const Diagonale& d) { return flux << d.nom; }

class GrilleDiagonaleTest : public ::testing::TestWithParam<Diagonale> {};

TEST_P(GrilleDiagonaleTest, quatre_jetons_en_diagonale_forment_un_alignement) {
    const Diagonale& d = GetParam();
    Grille grille;
    int ligne = 0;
    for (int i = 0; i < 4; ++i) {
        for (int k = 0; k < d.cales[i]; ++k) {
            grille.jouer(d.colonnes[i], Jeton::Jaune);
        }
        ligne = grille.jouer(d.colonnes[i], Jeton::Rouge);
    }

    EXPECT_TRUE(grille.aligneQuatre(ligne, d.colonnes[3]));
}

INSTANTIATE_TEST_SUITE_P(EX14, GrilleDiagonaleTest,
                         ::testing::Values(Diagonale{"montante", {0, 1, 2, 3}, {0, 1, 2, 3}},
                                           Diagonale{"descendante", {3, 4, 5, 6}, {3, 2, 1, 0}},
                                           Diagonale{"montante_au_bord", {3, 4, 5, 6}, {0, 1, 2, 3}}),
                         [](const ::testing::TestParamInfo<Diagonale>& info) { return std::string(info.param.nom); });

TEST(GrilleTest, une_grille_remplie_est_pleine) {
    Grille grille;
    EXPECT_FALSE(grille.estPleine());
    for (int colonne = 0; colonne < Grille::COLONNES; ++colonne) {
        for (int ligne = 0; ligne < Grille::LIGNES; ++ligne) {
            grille.jouer(colonne, (ligne + colonne / 2) % 2 ? Jeton::Jaune : Jeton::Rouge);
        }
    }

    EXPECT_TRUE(grille.estPleine());
}

// branche non couverte relevée par gcovr (Grille.cpp, ligne 50)
TEST(GrilleJouerTest, jouer_un_jeton_vide_est_refuse) {
    Grille grille;

    EXPECT_THROW(grille.jouer(0, Jeton::Vide), std::invalid_argument);
}
