// Dernière génération/modification faite par l'IA Claude le 09/10/2026 16:47:56
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

// --- Fin de partie : victoire verticale, match nul, partie terminée -----------

TEST_F(PartieTest, quatre_jetons_empiles_font_gagner_le_joueur) {
    for (int i = 0; i < 3; ++i) {
        partie.jouer(0);  // Alice
        partie.jouer(1);  // Bob
    }

    partie.jouer(0);

    EXPECT_EQ(partie.statut(), StatutPartie::Gagnee);
    EXPECT_EQ(partie.vainqueur()->nom(), "Alice");
}

// Séquence de 42 coups sans aucun alignement de 4 (EX-16)
constexpr int SEQUENCE_NULLE[] = {5, 4, 5, 0, 6, 2, 4, 5, 5, 0, 4, 1, 1, 0, 4, 5, 6, 5, 3, 1, 1, 2, 2, 6, 2, 6, 6, 3, 6, 2, 0, 3, 0, 3, 3, 4, 3, 1, 4, 2, 1, 0};

TEST_F(PartieTest, une_grille_pleine_sans_alignement_donne_un_match_nul) {
    for (int colonne : SEQUENCE_NULLE) {
        ASSERT_EQ(partie.statut(), StatutPartie::EnCours);
        partie.jouer(colonne);
    }

    EXPECT_EQ(partie.statut(), StatutPartie::Nulle);
    EXPECT_FALSE(partie.vainqueur().has_value());
    EXPECT_EQ(partie.nombreDeCoups(), 42);
}

TEST_F(PartieTest, jouer_apres_une_victoire_leve_PartieTerminee) {
    for (int i = 0; i < 3; ++i) {
        partie.jouer(0);
        partie.jouer(1);
    }
    partie.jouer(0);  // victoire d'Alice

    EXPECT_THROW(partie.jouer(5), p4::domaine::PartieTerminee);
    EXPECT_EQ(partie.nombreDeCoups(), 7);
}

// Test ajouté après le test de mutation : le mutant « match nul vérifié avant la
// victoire » survivait. Ici, le 42e et dernier coup (Bob) aligne 4 jetons : c'est une victoire.
constexpr int SEQUENCE_VICTOIRE_AU_DERNIER_COUP[] = {5, 6, 1, 3, 6, 1, 4, 4, 5, 1, 0, 3, 1, 5, 2, 0, 0, 6, 1, 0, 4, 5, 2, 0, 4, 5, 0, 4, 2, 4, 5, 1, 6, 2, 6, 2, 2, 3, 3, 6, 3, 3};

TEST_F(PartieTest, une_victoire_sur_le_dernier_coup_est_une_victoire_et_pas_un_nul) {
    for (int colonne : SEQUENCE_VICTOIRE_AU_DERNIER_COUP) {
        ASSERT_EQ(partie.statut(), StatutPartie::EnCours);
        partie.jouer(colonne);
    }

    EXPECT_TRUE(partie.grille().estPleine());
    EXPECT_EQ(partie.statut(), StatutPartie::Gagnee);
    EXPECT_EQ(partie.vainqueur()->nom(), "Bob");
}
