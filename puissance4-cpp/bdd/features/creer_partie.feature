# language: fr
# Dernière génération/modification faite par l'IA Claude le 13/10/2026 21:03:41
@iteration1
Fonctionnalité: Créer une partie
  Afin de jouer au Puissance 4 à deux
  En tant que joueur
  Je veux créer une partie en donnant le nom des deux joueurs

  @EX-01
  Scénario: Une nouvelle partie est en cours avec une grille vide
    Quand "Alice" et "Bob" créent une partie
    Alors la partie est en cours
    Et la grille est vide
    Et c'est au tour de "Alice"
    Et "Alice" joue les jetons rouges et "Bob" les jetons jaunes

  @EX-04
  Scénario: Les parties sont numérotées dans l'ordre de création
    Soit une partie entre "Alice" et "Bob"
    Quand "Chloé" et "David" créent une partie
    Alors l'identifiant de la partie est "P0002"

  @EX-02 @EX-05
  Plan du scénario: Des noms de joueurs invalides sont refusés
    Quand "<joueur1>" et "<joueur2>" créent une partie
    Alors la création est refusée avec le message "<message>"

    Exemples:
      | joueur1               | joueur2 | message                                            |
      |                       | Bob     | Le nom du joueur est obligatoire                   |
      | Alice                 |         | Le nom du joueur est obligatoire                   |
      | Abcdefghijabcdefghijk | Bob     | Le nom du joueur ne doit pas dépasser 20 caractères |
      | Alice                 | alice   | Les deux joueurs doivent avoir des noms différents |
