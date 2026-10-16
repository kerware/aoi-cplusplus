Language: French
# Dernière génération/modification faite par l'IA Claude le 13/10/2026 21:06:41
# Le même type de test, en style BDD et en français : en-têtes traduits (Language: French) et préfixes
# Soit / Quand / Alors / Et, ignorés lors de la recherche du mot-clé (« Soit une partie entre … »
# appelle le mot-clé « une partie entre "${joueur1}" et "${joueur2}" »).

*** Paramètres ***
Documentation     Fin de partie en style BDD (comparer avec bdd/features/fin_de_partie.feature).
Ressource         ressources/puissance4.resource
Mise en place de suite    Ouvrir La Session API
Étiquette de test    bdd    iteration6


*** Cas de test ***
Une victoire horizontale
    [Étiquette]    EX-14    EX-15
    Soit une partie entre "Alice" et "Bob"
    Quand les joueurs jouent la séquence "0 0 1 1 2 2 3"
    Alors la partie est gagnée par "Alice"

Un coup après la victoire est refusé
    [Étiquette]    EX-17
    Soit une partie entre "Alice" et "Bob"
    Et les joueurs jouent la séquence "0 1 0 1 0 1 0"
    Quand le joueur courant joue dans la colonne 3
    Alors le coup est refusé avec le code 409
