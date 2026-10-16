# Dernière génération/modification faite par l'IA Claude le 13/10/2026 21:06:41
*** Settings ***
Documentation     Mots-clés de l'API Puissance 4, en deux couches :
...               - mots-clés MÉTIER (« Créer Une Partie », « La Partie Doit Être Gagnée Par ») : ce qu'écrit le testeur ;
...               - mots-clés TECHNIQUES (session HTTP, JSON) : RequestsLibrary, cachés derrière les premiers.
...               L'adresse du serveur vient de la variable P4_URL (robot --variable P4_URL:http://…).
Library           RequestsLibrary
Library           Collections
Library           String


*** Variables ***
${P4_URL}         http://127.0.0.1:8080
${SESSION}        p4


*** Keywords ***
# --- Mots-clés techniques ----------------------------------------------------------------
Ouvrir La Session API
    [Documentation]    Ouvre la session HTTP et vérifie que le serveur répond (point de santé).
    Create Session    ${SESSION}    ${P4_URL}    disable_warnings=1
    ${sante}=    GET On Session    ${SESSION}    /api/sante    expected_status=200
    Log    Serveur ${sante.json()}[version] (commit ${sante.json()}[commit], build ${sante.json()}[build])

Consulter La Partie
    [Arguments]    ${id}
    ${reponse}=    GET On Session    ${SESSION}    /api/parties/${id}    expected_status=200
    RETURN    ${reponse.json()}

Jouer Dans La Colonne
    [Documentation]    Joue un coup ; échoue si le code HTTP n'est pas celui attendu (200 par défaut).
    [Arguments]    ${id}    ${colonne}    ${statut_attendu}=200
    ${corps}=    Create Dictionary    colonne=${${colonne}}
    ${reponse}=    POST On Session    ${SESSION}    /api/parties/${id}/coups    json=${corps}
    ...    expected_status=${statut_attendu}
    RETURN    ${reponse}

# --- Mots-clés métier --------------------------------------------------------------------
Créer Une Partie
    [Arguments]    ${joueur1}    ${joueur2}
    ${corps}=    Create Dictionary    joueur1=${joueur1}    joueur2=${joueur2}
    ${reponse}=    POST On Session    ${SESSION}    /api/parties    json=${corps}    expected_status=201
    RETURN    ${reponse.json()}[id]

Jouer La Séquence
    [Documentation]    « 0 1 0 1 » : coups joués à tour de rôle, tous acceptés.
    [Arguments]    ${id}    ${sequence}
    @{colonnes}=    Split String    ${sequence}
    FOR    ${colonne}    IN    @{colonnes}
        Jouer Dans La Colonne    ${id}    ${colonne}
    END

La Partie Doit Être En Cours
    [Arguments]    ${id}
    ${partie}=    Consulter La Partie    ${id}
    Should Be Equal    ${partie}[statut]    EN_COURS

Le Joueur Courant Doit Être
    [Arguments]    ${id}    ${nom}
    ${partie}=    Consulter La Partie    ${id}
    Should Be Equal    ${partie}[joueurCourant]    ${nom}

La Partie Doit Être Gagnée Par
    [Arguments]    ${id}    ${nom}
    ${partie}=    Consulter La Partie    ${id}
    Should Be Equal    ${partie}[statut]    GAGNEE
    Should Be Equal    ${partie}[vainqueur]    ${nom}

La Partie Doit Être Nulle
    [Arguments]    ${id}
    ${partie}=    Consulter La Partie    ${id}
    Should Be Equal    ${partie}[statut]    NULLE
    Should Be Equal    ${partie}[vainqueur]    ${None}

Le Coup Dans La Colonne Doit Donner Le Statut
    [Documentation]    Mot-clé de modèle (Test Template) : une nouvelle partie par ligne de données.
    [Arguments]    ${colonne}    ${statut}
    ${id}=    Créer Une Partie    Alice    Bob
    Jouer Dans La Colonne    ${id}    ${colonne}    ${statut}

# --- Mots-clés à arguments intégrés (style BDD, voir puissance4_bdd.robot) ----------------
une partie entre "${joueur1}" et "${joueur2}"
    ${id}=    Créer Une Partie    ${joueur1}    ${joueur2}
    Set Test Variable    ${PARTIE}    ${id}

les joueurs jouent la séquence "${sequence}"
    Jouer La Séquence    ${PARTIE}    ${sequence}

la partie est gagnée par "${nom}"
    La Partie Doit Être Gagnée Par    ${PARTIE}    ${nom}

le joueur courant joue dans la colonne ${colonne}
    ${reponse}=    Jouer Dans La Colonne    ${PARTIE}    ${colonne}    statut_attendu=any
    Set Test Variable    ${REPONSE}    ${reponse}

le coup est refusé avec le code ${code}
    Should Be Equal As Integers    ${REPONSE.status_code}    ${code}
