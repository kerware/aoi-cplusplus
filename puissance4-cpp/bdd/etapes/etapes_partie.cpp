// Dernière génération/modification faite par l'IA Claude le 13/10/2026 21:12:10
// ---------------------------------------------------------------------------
// Code glue des scénarios Gherkin de bdd/features (itération 6).
// Point de départ : squelette généré par gherkin-glue (cible puissance4_bdd_glue), puis complété.
// L'état du scénario (Monde) est dans Monde.hpp, partagé par tous les fichiers d'étapes.
// Les étapes pilotent le PORT D'ENTRÉE de l'hexagone (ServicePartie) avec les vrais adaptateurs de
// sortie en mémoire : test d'acceptation rapide, sans réseau ni IHM.
// Retouche du squelette : « un jeton rouge… » et « un jeton jaune… » fusionnées en « un jeton {word}… ».
// ---------------------------------------------------------------------------
#include "Monde.hpp"
#include "cuke/Cuke.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <string>

using namespace p4;
using bdd::Monde;

namespace {

std::string enMajuscules(std::string texte) {
    std::transform(texte.begin(), texte.end(), texte.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return texte;
}

} // namespace

// --- Créer une partie (itération 1) ----------------------------------------------------

QUAND("{string} et {string} créent une partie") {
    ctx.obtenir<Monde>().creer(arg.texte(0), arg.texte(1));
}

SOIT("une partie entre {string} et {string}") {
    auto& m = ctx.obtenir<Monde>();
    m.creer(arg.texte(0), arg.texte(1));
    ASSERT_TRUE(m.id) << "création refusée : " << m.message.value_or("");
}

ALORS("la partie est en cours") {
    EXPECT_EQ(ctx.obtenir<Monde>().partie().statut(), domaine::StatutPartie::EnCours);
}

ALORS("la grille est vide") {
    EXPECT_TRUE(ctx.obtenir<Monde>().partie().grille().estVide());
}

ALORS("c'est au tour de {string}") {
    EXPECT_EQ(ctx.obtenir<Monde>().partie().joueurCourant().nom(), arg.texte(0));
}

ALORS("{string} joue les jetons rouges et {string} les jetons jaunes") {
    const domaine::Partie partie = ctx.obtenir<Monde>().partie();
    EXPECT_EQ(partie.joueur1().nom(), arg.texte(0));
    EXPECT_EQ(partie.joueur1().couleur(), domaine::Jeton::Rouge);
    EXPECT_EQ(partie.joueur2().nom(), arg.texte(1));
    EXPECT_EQ(partie.joueur2().couleur(), domaine::Jeton::Jaune);
}

ALORS("l'identifiant de la partie est {string}") {
    EXPECT_EQ(ctx.obtenir<Monde>().id.value_or("aucune partie"), arg.texte(0));
}

ALORS("la création est refusée avec le message {string}") {
    auto& m = ctx.obtenir<Monde>();
    ASSERT_EQ(m.refus.value_or("acceptée"), "création");
    EXPECT_EQ(m.message.value_or(""), arg.texte(0));
    EXPECT_EQ(m.repository.nombreDeParties(), 0u) << "aucune partie ne doit être créée";
}

// --- Jouer un coup (itération 2) --------------------------------------------------------

QUAND("le joueur courant joue dans la colonne {int}") {
    auto& m = ctx.obtenir<Monde>();
    m.jouer(m.id.value(), arg.entier(0));
}

QUAND("on joue dans la colonne {int} de la partie {string}") {
    ctx.obtenir<Monde>().jouer(arg.texte(1), arg.entier(0));
}

QUAND("les joueurs jouent la séquence {string}") {
    ctx.obtenir<Monde>().jouerSequence(arg.texte(0));
}

SOIT("les joueurs ont joué la séquence {string}") {
    ctx.obtenir<Monde>().jouerSequence(arg.texte(0));
}

ALORS("un jeton {word} occupe la ligne {int} de la colonne {int}") {
    const domaine::Partie partie = ctx.obtenir<Monde>().partie();
    const domaine::Jeton jeton = partie.grille().caseEn(arg.entier(1), arg.entier(2));
    EXPECT_EQ(std::string(domaine::versTexte(jeton)), enMajuscules(arg.texte(0)));
}

ALORS("le coup est refusé : {string}") {
    auto& m = ctx.obtenir<Monde>();
    ASSERT_TRUE(m.refus) << "le coup a été accepté";
    EXPECT_EQ(*m.refus, arg.texte(0)) << m.message.value_or("");
}

ALORS("la partie est inchangée") {
    auto& m = ctx.obtenir<Monde>();
    ASSERT_TRUE(m.avantLeCoup);
    const domaine::Partie apres = m.partie();
    EXPECT_EQ(apres.nombreDeCoups(), m.avantLeCoup->nombreDeCoups());
    EXPECT_EQ(apres.grille().nombreDeJetons(), m.avantLeCoup->grille().nombreDeJetons());
    EXPECT_EQ(apres.joueurCourant().nom(), m.avantLeCoup->joueurCourant().nom());
    EXPECT_EQ(apres.statut(), m.avantLeCoup->statut());
}

// --- Fin de partie (itération 3) --------------------------------------------------------

ALORS("la partie est gagnée par {string}") {
    const domaine::Partie partie = ctx.obtenir<Monde>().partie();
    EXPECT_EQ(partie.statut(), domaine::StatutPartie::Gagnee);
    ASSERT_TRUE(partie.vainqueur());
    EXPECT_EQ(partie.vainqueur()->nom(), arg.texte(0));
}

ALORS("la partie est nulle") {
    EXPECT_EQ(ctx.obtenir<Monde>().partie().statut(), domaine::StatutPartie::Nulle);
}

ALORS("la partie n'a pas de vainqueur") {
    EXPECT_FALSE(ctx.obtenir<Monde>().partie().vainqueur());
}
