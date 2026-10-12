// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
#include "domaine/Joueur.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

using p4::domaine::Jeton;
using p4::domaine::Joueur;

TEST(JoueurTest, un_joueur_a_un_nom_et_une_couleur) {
    const Joueur alice("Alice", Jeton::Rouge);

    EXPECT_EQ(alice.nom(), "Alice");
    EXPECT_EQ(alice.couleur(), Jeton::Rouge);
}

TEST(JoueurTest, les_espaces_autour_du_nom_sont_retires) {
    const Joueur bob("  Bob \t", Jeton::Jaune);

    EXPECT_EQ(bob.nom(), "Bob");
}

TEST(JoueurTest, le_nom_est_obligatoire) {
    EXPECT_THROW(Joueur("", Jeton::Rouge), std::invalid_argument);
}

TEST(JoueurTest, un_nom_compose_uniquement_d_espaces_est_refuse) {
    EXPECT_THROW(Joueur("    ", Jeton::Rouge), std::invalid_argument);
}

TEST(JoueurTest, un_nom_de_20_caracteres_est_accepte) {
    EXPECT_NO_THROW(Joueur(std::string(20, 'a'), Jeton::Rouge));
}

TEST(JoueurTest, un_nom_de_21_caracteres_est_refuse) {
    EXPECT_THROW(Joueur(std::string(21, 'a'), Jeton::Rouge), std::invalid_argument);
}

TEST(JoueurTest, les_caracteres_accentues_comptent_pour_un) {
    // "Éloïse" : 6 caractères mais 8 octets en UTF-8
    EXPECT_NO_THROW(Joueur("ÉloïseÉloïseÉloïseÉl", Jeton::Jaune));  // 20 caractères
}

TEST(JoueurTest, un_joueur_ne_peut_pas_avoir_la_couleur_vide) {
    EXPECT_THROW(Joueur("Alice", Jeton::Vide), std::invalid_argument);
}
