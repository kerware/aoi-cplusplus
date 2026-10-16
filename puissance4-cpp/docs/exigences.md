<!-- Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:06:04 -->
# Cahier d'exigences — Puissance 4 (itérations 1 à 5)

Source : `generation/referentiel_it2.py` (même contenu que l'import Squash TM).

| Réf. | Exigence | Description | Criticité | Itération |
|---|---|---|---|---|
| EX-01 | Créer une partie | Un utilisateur crée une partie en donnant le nom des deux joueurs. Le joueur 1 a les jetons rouges et commence, le joueur 2 a les jetons jaunes. La partie créée est EN_COURS avec une grille vide de 6 lignes et 7 colonnes. L'API répond 201 avec l'adresse de la partie (Location). | critique | 1 |
| EX-02 | Nom d'un joueur | Le nom d'un joueur comporte de 1 à 20 caractères, espaces de début et de fin retirés. Sinon l'API répond 400 avec un message explicite. | majeure | 1 |
| EX-03 | Consulter une partie | On consulte une partie par son identifiant. Une partie inconnue donne 404. | majeure | 1 |
| EX-04 | Numérotation des parties | Les parties sont numérotées P0001, P0002… dans l'ordre de leur création, sans trou dans la numérotation. | mineure | 1 |
| EX-05 | Noms différents | Les deux joueurs ont des noms différents (casse et espaces de bord ignorés). Sinon 400 « Les deux joueurs doivent avoir des noms différents ». | mineure | 1 |
| EX-06 | Jouer un coup | Le joueur courant choisit une colonne : son jeton tombe dans la case libre la plus basse de cette colonne. | critique | 2 |
| EX-07 | Alternance des joueurs | Après un coup valide, la main passe à l'autre joueur. | critique | 2 |
| EX-08 | Colonne valide | La colonne est un nombre entier de 0 à 6. Sinon le coup est refusé : 400 « Colonne invalide : doit être un entier entre 0 et 6 », la partie est inchangée. | majeure | 2 |
| EX-09 | Colonne pleine | Un coup dans une colonne pleine est refusé : 409 « Colonne pleine », la grille et le joueur courant sont inchangés. | majeure | 2 |
| EX-10 | Coup dans une partie inconnue | Jouer dans une partie inexistante donne 404. | majeure | 2 |
| EX-11 | Réponse à un coup | La réponse à un coup valide (200) contient la partie à jour : grille, joueur courant, nombre de coups et dernier coup (colonne, ligne, couleur, joueur). | mineure | 2 |
| EX-12 | Jouer depuis l'IHM | Dans l'IHM, le joueur clique sur une colonne pour jouer ; la grille, le joueur courant et le nombre de coups sont mis à jour. Une colonne pleine ne peut plus être choisie. | majeure | 2 |
| EX-13 | Temps de réponse d'un coup | Un coup est traité en moins de 50 ms pour 95 % des requêtes, avec 50 parties jouées simultanément. | mineure | 2 |
| EX-14 | Victoire | 4 jetons de même couleur alignés (horizontale, verticale, diagonales) font gagner le joueur. | critique | 3 |
| EX-15 | Partie gagnée | La partie gagnée a le statut GAGNEE et indique le vainqueur (API, IHM). | majeure | 3 |
| EX-16 | Match nul | Grille pleine sans alignement : statut NULLE, pas de vainqueur. | majeure | 3 |
| EX-17 | Partie terminée | Aucun coup n'est accepté dans une partie terminée : 409 « Partie terminée : plus aucun coup n'est accepté ». | majeure | 3 |
| EX-18 | État final dans l'IHM | À la fin de la partie, l'IHM affiche « Partie terminée — victoire de X » ou « Partie terminée — match nul » à la place de « au tour de … ». | mineure | 4 |
| EX-19 | Coups simultanés | Des coups envoyés simultanément sur une même partie sont traités l'un après l'autre : chaque coup accepté (200) est enregistré. | majeure | 4 |
| EX-20 | Client bureau | Un client Qt (Windows, Linux) permet de créer une partie et de jouer, avec les mêmes règles et messages que l'IHM web. | majeure | 4 |
| EX-21 | Traçabilité du binaire | Le point de santé `GET /api/sante` indique la version du produit, le commit Git et le numéro de build de l'intégration continue qui a produit l'exécutable ; l'IHM les affiche en pied de page. Un binaire compilé hors CI indique « local » et le build 0. | mineure | 5 |

**Itération 5** : EX-21 (traçabilité du binaire) — la CI grave le commit et le numéro de build.

**Itération 4** : EX-13 (performance) passe au statut *approuvée* et devient testable : environnement de référence =
serveur **Release** dans Docker, injecteur dans le même réseau Docker, 50 utilisateurs virtuels.
