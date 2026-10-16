# Dernière génération/modification faite par l'IA Claude le 13/10/2026 21:06:41
*** Settings ***
Documentation     Tests d'acceptation de l'API Puissance 4, pilotés par mots-clés (keyword-driven testing).
...               Les tests n'emploient que des mots-clés métier ; le HTTP est caché dans la ressource.
...               robot --variable P4_URL:http://127.0.0.1:8080 --outputdir resultats tests/systeme/robot
Resource          ressources/puissance4.resource
Suite Setup       Ouvrir La Session API
Test Tags         api    iteration6


*** Test Cases ***
Une nouvelle partie est en cours et Alice commence
    [Tags]    EX-01
    ${partie}=    Créer Une Partie    Alice    Bob
    La Partie Doit Être En Cours    ${partie}
    Le Joueur Courant Doit Être    ${partie}    Alice

Quatre jetons alignés verticalement font gagner Alice
    [Tags]    EX-14    EX-15
    ${partie}=    Créer Une Partie    Alice    Bob
    Jouer La Séquence    ${partie}    0 1 0 1 0 1 0
    La Partie Doit Être Gagnée Par    ${partie}    Alice

Une grille pleine sans alignement donne un match nul
    [Tags]    EX-16
    ${partie}=    Créer Une Partie    Alice    Bob
    Jouer La Séquence    ${partie}
    ...    5 4 5 0 6 2 4 5 5 0 4 1 1 0 4 5 6 5 3 1 1 2 2 6 2 6 6 3 6 2 0 3 0 3 3 4 3 1 4 2 1 0
    La Partie Doit Être Nulle    ${partie}

Aucun coup n'est accepté après la fin de la partie
    [Tags]    EX-17
    ${partie}=    Créer Une Partie    Alice    Bob
    Jouer La Séquence    ${partie}    0 1 0 1 0 1 0
    Jouer Dans La Colonne    ${partie}    3    statut_attendu=409

Les colonnes hors de la grille sont refusées
    [Documentation]    Test piloté par les données : une ligne = un cas (partitions et limites d'EX-08).
    [Tags]    EX-08
    [Template]    Le Coup Dans La Colonne Doit Donner Le Statut
    -1    400
    0     200
    6     200
    7     400
