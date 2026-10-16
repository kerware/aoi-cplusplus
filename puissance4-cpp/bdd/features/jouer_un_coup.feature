# language: fr
# Dernière génération/modification faite par l'IA Claude le 13/10/2026 21:03:41
@iteration2
Fonctionnalité: Jouer un coup
  Le joueur courant choisit une colonne ; son jeton tombe dans la case libre la plus basse.

  Contexte:
    Soit une partie entre "Alice" et "Bob"

  @EX-06 @EX-07
  Scénario: Le jeton tombe en bas de la colonne et la main passe à l'adversaire
    Quand le joueur courant joue dans la colonne 3
    Alors un jeton rouge occupe la ligne 5 de la colonne 3
    Et c'est au tour de "Bob"

  @EX-06
  Scénario: Les jetons s'empilent dans une colonne
    Quand les joueurs jouent la séquence "3 3 3"
    Alors un jeton rouge occupe la ligne 5 de la colonne 3
    Et un jeton jaune occupe la ligne 4 de la colonne 3
    Et un jeton rouge occupe la ligne 3 de la colonne 3

  @EX-08
  Plan du scénario: Une colonne hors de la grille est refusée
    Quand le joueur courant joue dans la colonne <colonne>
    Alors le coup est refusé : "colonne invalide"
    Et la partie est inchangée

    Exemples:
      | colonne |
      | -1      |
      | 7       |

  @EX-09
  Scénario: Une colonne pleine est refusée
    Soit les joueurs ont joué la séquence "0 0 0 0 0 0"
    Quand le joueur courant joue dans la colonne 0
    Alors le coup est refusé : "colonne pleine"
    Et la partie est inchangée
    Et c'est au tour de "Alice"

  @EX-10
  Scénario: On ne joue pas dans une partie inconnue
    Quand on joue dans la colonne 3 de la partie "P9999"
    Alors le coup est refusé : "partie inconnue"
