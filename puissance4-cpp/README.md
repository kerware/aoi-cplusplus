<!-- Dernière génération/modification faite par l'IA Claude le 12/10/2026 23:09:26 -->
# Puissance 4 — projet fil rouge de la formation AOI

Serveur de jeu Puissance 4 en **C++20**, exposé par une **API REST**, construit en
**architecture hexagonale** (ports et adaptateurs). Il est compilé et testé sur
**Windows 11 (MSVC)** et **Linux (GCC, via Docker)**.

> **Itération 4 — tests système** (chapitre 4 de la formation). Base : itération 3 + corrigé du TP 3.
> Nouveautés : état final dans l'IHM (EX-18), **client bureau Qt** (EX-20), et toute l'infrastructure de
> tests système : API (GoogleTest, Bruno), IHM (Playwright, Selenium, QtTest), charge (k6, Gatling),
> environnement Docker Compose. Le TP 4 s'exécute sur ce projet.

## Architecture

```
            ADAPTATEURS D'ENTRÉE                    ADAPTATEURS DE SORTIE
   IHM HTML5 ──HTTP──► ServeurRest ──► [CreerPartie]      [PartieRepository] ◄── PartieRepositoryEnMemoire
                        (cpp-httplib)   [ConsulterPartie]  [GenerateurIdentifiant] ◄── GenerateurIdentifiantSequentiel
                                        [JouerCoup]
                                              │                      ▲
                                              ▼                      │
                                        ServicePartie  ──────────────┘
                                        Partie · Joueur · Grille · Jeton
                                              DOMAINE (aucune dépendance)
```

| Cible CMake          | Contenu                                   | Dépend de                     |
|----------------------|-------------------------------------------|-------------------------------|
| `p4_domaine`         | `src/domaine`, `src/application` (ports)  | **rien**                      |
| `p4_adaptateurs`     | `src/adaptateurs` (REST, mémoire)         | `p4_domaine`, httplib, json   |
| `puissance4_serveur` | `src/main.cpp` (composition root)         | `p4_adaptateurs`              |
| `tests_unitaires`    | `tests/unitaires` (GoogleTest + GMock)    | `p4_domaine` uniquement       |
| `tests_integration`  | `tests/integration` (vrai serveur HTTP)   | `p4_adaptateurs`              |

## Prérequis

| Plate-forme | Outils |
|-------------|--------|
| Windows 11  | Visual Studio 2022 ou Build Tools 2022 (charge « Développement C++ »), CMake ≥ 3.25, Git |
| Linux       | Docker Desktop (l'image `ubuntu:24.04` fournit GCC 13, CMake 3.28, Ninja) |

Les dépendances (cpp-httplib 0.15.3, nlohmann/json 3.11.3, GoogleTest 1.15.2) sont
téléchargées automatiquement par `FetchContent` à la première configuration.

## Compiler et tester

### Windows 11 (MSVC)

```powershell
cmake --workflow --preset windows-msvc          # configure + compile + 95 tests (+ 4 tests système ignorés sans serveur)
# ou étape par étape :
cmake --preset windows-msvc
cmake --build --preset windows-msvc
ctest --preset windows-msvc                      # tous les tests
ctest --preset windows-msvc-unitaires            # uniquement le label "unitaire"
```

### Linux (via Docker)

```powershell
docker build -t puissance4:iteration4 .          # compile + teste sous GCC ; échoue si un test échoue
docker run --rm -it -p 8080:8080 puissance4:iteration4
```

Shell interactif dans la chaîne de compilation Linux (sources montées) :

```powershell
docker build --target outils -t p4-outils .
docker run --rm -it -v "${PWD}:/src" p4-outils bash
# dans le conteneur :
cmake --workflow --preset linux-gcc
```

## Qualité : couverture et analyse statique (Linux, via Docker)

```powershell
docker build --target rapports --output type=local,dest=rapports .
```

Produit dans `rapports/` : `couverture/index.html` (gcovr, détail par fichier), `couverture-lignes.txt`,
`couverture-decisions.txt`, `couverture-branches.txt` (sans branches d'exceptions), `couverture-branches-brutes.txt`,
`couverture-cobertura.xml`, `cppcheck.txt` / `cppcheck.xml`, `clang-tidy.txt`.
Preset utilisé : `linux-gcc-couverture` (`--coverage -O0`).

## Lancer le serveur

```powershell
.\build\windows-msvc\src\Debug\puissance4_serveur.exe --port 8080
```

Ouvrir <http://localhost:8080> pour l'IHM HTML5. Arrêt propre : taper `q` puis Entrée.

## Tests système

Le serveur doit tourner (Windows : `build\windows-msvc\src\Release\puissance4_serveur.exe`, ou Docker :
`docker compose up -d serveur`). Son adresse est passée par la variable **`P4_URL`**.

| Type | Outil | Windows (PowerShell, depuis la racine) | Linux (Docker Compose) |
|---|---|---|---|
| API | GoogleTest | `$env:P4_URL="http://127.0.0.1:8080"; ctest --preset windows-msvc -L systeme` | `docker compose run --rm tests-cpp` |
| API | Bruno | `cd tests\systeme; npm ci; npm run api; npm run api:partitions` | `docker compose run --rm tests-api` |
| IHM web | Playwright | `cd tests\systeme; npx playwright test` (Edge) | `docker compose run --rm tests-ihm` (Chromium, Firefox) |
| IHM web | Selenium | `python -m pytest tests\systeme\selenium -v` | — |
| IHM bureau | QtTest | `cd qt-client; cmake --workflow --preset windows-mingw-qt` | `docker compose run --rm tests-qt` |
| Charge | k6 | `docker run --rm -i -e P4_URL=http://host.docker.internal:8080 grafana/k6:2.3.0 run - < tests\systeme\charge\charge-coups.js` | `docker compose run --rm charge` |
| Charge | Gatling | `cd tests\systeme\charge-gatling; mvn gatling:test "-Dp4.url=http://127.0.0.1:8080"` | `docker compose run --rm charge-gatling` |

Rapports : `tests/systeme/resultats/` (JUnit, rapport HTML Playwright, résumé k6) et
`tests/systeme/charge-gatling/target/gatling/` (rapport HTML Gatling).

> Les mesures de charge se font sur un serveur **Release**, avec l'injecteur dans le même réseau que le
> serveur (Docker Compose). Depuis Windows vers un conteneur, le réseau NAT de Docker Desktop fausse la mesure.

## API REST (itération 4)

| Méthode | Chemin               | Corps                                   | Réponses |
|---------|----------------------|-----------------------------------------|----------|
| GET     | `/api/sante`         | —                                       | 200 `{"statut":"OK"}` |
| POST    | `/api/parties`       | `{"joueur1":"Alice","joueur2":"Bob"}`   | 201 + `Location` · 400 si invalide |
| GET     | `/api/parties/{id}`  | —                                       | 200 · 404 si inconnue |
| POST    | `/api/parties/{id}/coups` | `{"colonne":3}`                   | 200 · 400 colonne invalide · 404 · 409 colonne pleine ou partie terminée |

```powershell
curl.exe -s -X POST http://localhost:8080/api/parties -H "Content-Type: application/json" -d '{\"joueur1\":\"Alice\",\"joueur2\":\"Bob\"}'
```

## Les tests de l'itération 4

| Niveau        | Exécutable          | Nombre | Outils |
|---------------|---------------------|--------|--------|
| Unitaire      | `tests_unitaires`   | 67     | GoogleTest, GoogleMock (bouchons des ports de sortie) |
| Intégration   | `tests_integration` | 28     | GoogleTest + client HTTP cpp-httplib sur un port libre |
| Système API   | `tests_systeme`     | 4      | GoogleTest + client HTTP contre le serveur déployé (`P4_URL`) |
| Système API   | collection Bruno    | 6 + 7 cas CSV | Bruno CLI 4.2 |
| Système IHM   | Playwright          | 5 (× 2 navigateurs sous Linux) | Playwright 1.64 |
| Système IHM   | Selenium            | 2      | Selenium 4.41 + pytest (comparaison) |
| IHM bureau    | `tst_partievue`, `tst_fenetre` | 2 exécutables QtTest | QtTest (Qt 6.11 Windows, 6.4 Linux) |
| Charge        | k6, Gatling         | 1 scénario chacun | k6 2.3, Gatling 3.11 |

## Feuille de route du fil rouge

| Chapitre de formation             | Itération | Apport |
|-----------------------------------|-----------|--------|
| 1. Démarche de tests              | 1         | Squelette multi-plateforme, premiers tests |
| 2. Analyse et conception          | 2         | Jouer un coup, exigences et référentiel Squash-TM |
| 3. Tests unitaires                | 3         | Victoire / match nul, TDD, GMock, gcov, cppcheck |
| 4. Tests système                  | 4         | Playwright, Bruno, k6, client Qt + QtTest |
| 5. Intégration continue           | 5         | GitHub Actions / Jenkins, rapports |
