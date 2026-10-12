// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#include "adaptateurs/sortie/memoire/GenerateurIdentifiantSequentiel.hpp"

#include <gtest/gtest.h>

using p4::adaptateurs::GenerateurIdentifiantSequentiel;

TEST(GenerateurIdentifiantSequentielTest, les_identifiants_se_suivent_sur_4_chiffres) {
    GenerateurIdentifiantSequentiel generateur;

    EXPECT_EQ(generateur.suivant(), "P0001");
    EXPECT_EQ(generateur.suivant(), "P0002");
}

TEST(GenerateurIdentifiantSequentielTest, au_dela_de_9999_le_numero_s_allonge) {
    GenerateurIdentifiantSequentiel generateur;
    for (int i = 0; i < 9999; ++i) {
        generateur.suivant();
    }

    EXPECT_EQ(generateur.suivant(), "P10000");
}
