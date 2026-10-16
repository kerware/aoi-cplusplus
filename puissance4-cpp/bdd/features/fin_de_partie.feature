# language: fr
# Dernière génération/modification faite par l'IA Claude le 13/10/2026 21:03:41
@iteration3
Fonctionnalité: Fin de partie
  La partie se termine par une victoire (quatre jetons alignés) ou par un match nul (grille pleine).

  Contexte:
    Soit une partie entre "Alice" et "Bob"

  @EX-14 @EX-15
  Plan du scénario: Quatre jetons alignés font gagner le joueur
    Quand les joueurs jouent la séquence "<sequence>"
    Alors la partie est gagnée par "<vainqueur>"

    Exemples:
      | alignement       | sequence                | vainqueur |
      | horizontal       | 0 0 1 1 2 2 3           | Alice     |
      | vertical         | 0 1 0 1 0 1 0           | Alice     |
      | diagonal montant | 0 1 1 2 2 3 2 3 3 6 3   | Alice     |
      | du second joueur | 6 0 6 1 6 2 5 3         | Bob       |

  @EX-16
  Scénario: Une grille pleine sans alignement donne un match nul
    Quand les joueurs jouent la séquence "5 4 5 0 6 2 4 5 5 0 4 1 1 0 4 5 6 5 3 1 1 2 2 6 2 6 6 3 6 2 0 3 0 3 3 4 3 1 4 2 1 0"
    Alors la partie est nulle
    Et la partie n'a pas de vainqueur

  @EX-17
  Scénario: Aucun coup n'est accepté après la fin de la partie
    Soit les joueurs ont joué la séquence "0 1 0 1 0 1 0"
    Quand le joueur courant joue dans la colonne 3
    Alors le coup est refusé : "partie terminée"
    Et la partie est inchangée
