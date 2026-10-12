// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#include "adaptateurs/sortie/memoire/PartieRepositoryEnMemoire.hpp"

#include <gtest/gtest.h>

using p4::adaptateurs::PartieRepositoryEnMemoire;
using p4::domaine::Partie;

TEST(PartieRepositoryEnMemoireTest, une_partie_sauvegardee_est_retrouvee_par_son_id) {
    PartieRepositoryEnMemoire repository;
    repository.sauvegarder(Partie("P0001", "Alice", "Bob"));

    const auto trouvee = repository.trouverParId("P0001");

    ASSERT_TRUE(trouvee.has_value());
    EXPECT_EQ(trouvee->joueur2().nom(), "Bob");
}

TEST(PartieRepositoryEnMemoireTest, un_id_inconnu_ne_retourne_rien) {
    const PartieRepositoryEnMemoire repository;

    EXPECT_FALSE(repository.trouverParId("P0404").has_value());
}

TEST(PartieRepositoryEnMemoireTest, sauvegarder_deux_fois_la_meme_partie_ne_la_duplique_pas) {
    PartieRepositoryEnMemoire repository;
    repository.sauvegarder(Partie("P0001", "Alice", "Bob"));
    repository.sauvegarder(Partie("P0001", "Alice", "Bob"));

    EXPECT_EQ(repository.nombreDeParties(), 1u);
}
