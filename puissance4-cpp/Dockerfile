# Dernière génération/modification faite par l'IA Claude le 13/10/2026 19:53:26
# ---------------------------------------------------------------------------
# Compilation et tests Linux du projet Puissance 4, depuis Windows + Docker Desktop ou depuis la CI
#
#   docker build -t puissance4:iteration5 .            # compile + teste + image d'exécution
#   docker run --rm -p 8080:8080 puissance4:iteration5 # lance le serveur
#   docker build --target outils -t p4-outils .        # image de la chaîne de compilation
#   docker build --target rapports --output type=local,dest=rapports .
#                                                      # couverture gcov + cppcheck + clang-tidy -> ./rapports
#   docker build --target resultats-tests --output type=local,dest=resultats .
#                                                      # rapport JUnit des tests, MÊME s'ils échouent (CI)
#
# Identification du build (EX-21) : --build-arg P4_COMMIT=<sha> --build-arg P4_BUILD=<numéro>
# ---------------------------------------------------------------------------

# --- Étape 1 : chaîne de compilation Linux (GCC 13, CMake 3.28, Ninja) ----------
FROM ubuntu:24.04 AS outils
RUN apt-get update \
 && apt-get install -y --no-install-recommends \
        build-essential cmake ninja-build git ca-certificates \
 && rm -rf /var/lib/apt/lists/*
WORKDIR /src

# --- Étape 2 : compilation (sans exécuter les tests) --------------------------------
# Copies ciblées : modifier un test Playwright ou un script k6 ne recompile pas le C++.
FROM outils AS compilation
ARG P4_COMMIT=local
ARG P4_BUILD=0
ENV P4_COMMIT=${P4_COMMIT} P4_BUILD=${P4_BUILD}
COPY CMakeLists.txt CMakePresets.json ./
COPY src src
COPY web web
COPY tests/CMakeLists.txt tests/
COPY tests/unitaires tests/unitaires
COPY tests/integration tests/integration
COPY tests/systeme/ApiSystemeTest.cpp tests/systeme/
RUN cmake --preset linux-gcc && cmake --build --preset linux-gcc

# --- Étape 2 bis : tests pour la CI -------------------------------------------------
# Les tests s'exécutent, leur résultat est TOUJOURS exporté (JUnit + code de retour) :
# c'est l'outil de CI qui décide ensuite (Jenkins : build « instable », GitHub Actions : échec).
# Les tests système (label « systeme ») sont exclus ici : ils ont besoin d'un serveur déployé.
FROM compilation AS tests-ci
RUN mkdir -p /resultats \
 && (ctest --preset linux-gcc -LE systeme --output-junit /resultats/ctest-linux.xml; echo $? > /resultats/ctest-code.txt)

FROM scratch AS resultats-tests
COPY --from=tests-ci /resultats /

# --- Étape 3 : tests (le build échoue si un test échoue) ----------------------------
FROM compilation AS build
RUN ctest --preset linux-gcc

# --- Étape 3 bis : compilation Release du serveur, APRÈS les tests ---------------
# Les mesures de performance (k6, Gatling) se font sur un binaire optimisé, jamais en Debug.
FROM build AS release
RUN cmake --preset linux-gcc-release && cmake --build --preset linux-gcc-release --target puissance4_serveur

# --- Étape 4 : qualité — couverture (gcov/gcovr) et analyse statique ------------
FROM outils AS qualite
RUN apt-get update \
 && apt-get install -y --no-install-recommends gcovr cppcheck clang-tidy \
 && rm -rf /var/lib/apt/lists/*
COPY CMakeLists.txt CMakePresets.json ./
COPY src src
COPY web web
COPY tests/CMakeLists.txt tests/
COPY tests/unitaires tests/unitaires
COPY tests/integration tests/integration
COPY tests/systeme/ApiSystemeTest.cpp tests/systeme/
# Compilation instrumentée (--coverage) puis exécution des tests : produit les .gcda.
# Un test en échec n'empêche pas de mesurer la couverture (c'est l'étape « tests » qui le signale).
RUN cmake --preset linux-gcc-couverture && cmake --build --preset linux-gcc-couverture \
 && (ctest --preset linux-gcc-couverture -LE systeme || true)
# Rapports de couverture (main.cpp exclu : il n'est pas exécuté par les tests)
RUN mkdir -p /rapports/couverture \
 && gcovr --root . --filter src/ --exclude src/main.cpp --decisions \
          --html-details /rapports/couverture/index.html \
          --cobertura /rapports/couverture-cobertura.xml \
          --json-summary /rapports/couverture-resume.json \
          --txt /rapports/couverture-lignes.txt build/linux-gcc-couverture \
 && gcovr --root . --filter src/ --exclude src/main.cpp --decisions --txt-metric decision \
          --txt /rapports/couverture-decisions.txt build/linux-gcc-couverture \
 && gcovr --root . --filter src/ --exclude src/main.cpp --txt-metric branch \
          --txt /rapports/couverture-branches-brutes.txt build/linux-gcc-couverture \
 && gcovr --root . --filter src/ --exclude src/main.cpp --txt-metric branch \
          --exclude-throw-branches --exclude-unreachable-branches \
          --txt /rapports/couverture-branches.txt build/linux-gcc-couverture
# Analyse statique : cppcheck (texte + XML pour SonarQube / Jenkins)
RUN cppcheck --enable=warning,style,performance,portability --std=c++20 --language=c++ -I src \
             --inline-suppr --suppress=missingIncludeSystem --quiet \
             --template='{file}:{line}: [{severity}/{id}] {message}' \
             --output-file=/rapports/cppcheck.txt src \
 && cppcheck --enable=warning,style,performance,portability --std=c++20 --language=c++ -I src \
             --inline-suppr --suppress=missingIncludeSystem --quiet --xml \
             --output-file=/rapports/cppcheck.xml src
# Analyse statique : clang-tidy (utilise compile_commands.json du build de couverture)
RUN clang-tidy -p build/linux-gcc-couverture --quiet \
        --checks='-*,bugprone-*,performance-*,modernize-use-nullptr,readability-else-after-return,-bugprone-easily-swappable-parameters' \
        $(find src -name '*.cpp') > /rapports/clang-tidy.txt 2>/dev/null || true

# --- Étape 4 bis : client Qt (Qt 6.4 d'Ubuntu) et ses tests QtTest -----------------
# Les tests unitaires Qt s'exécutent ici ; le test d'IHM système (tst_fenetre) est lancé par
# « docker compose run --rm tests-qt », contre le conteneur du serveur.
FROM outils AS qt
RUN apt-get update  && apt-get install -y --no-install-recommends qt6-base-dev libgl-dev fonts-dejavu-core  && rm -rf /var/lib/apt/lists/*
COPY qt-client /src/qt-client
WORKDIR /src/qt-client
RUN cmake --preset linux-qt && cmake --build --preset linux-qt && ctest --preset linux-qt -L qt-unitaire

# --- Étape 5 : export des rapports vers le poste (docker build --output) ---------
FROM scratch AS rapports
COPY --from=qualite /rapports /

# --- Étape 6 : injecteurs de tests système, autonomes (CI : aucun dossier monté) -----
# Utilisés par docker-compose.ci.yml ; les résultats sont récupérés par « docker cp ».
# Tous les injecteurs écrivent leurs résultats dans /resultats (lien depuis /tests/resultats).
FROM node:22-bookworm-slim AS tests-api
WORKDIR /tests
RUN mkdir /resultats && ln -s /resultats /tests/resultats
COPY tests/systeme/package.json tests/systeme/package-lock.json ./
RUN npm ci --no-audit --no-fund --loglevel=error
COPY tests/systeme/api api
WORKDIR /tests/api
CMD ["sh", "-c", "npx bru run parcours --env docker --reporter-junit ../resultats/bruno-parcours.xml; r1=$?; npx bru run partitions-colonne --env docker --csv-file-path donnees/colonnes.csv --reporter-junit ../resultats/bruno-partitions.xml; r2=$?; exit $((r1 + r2))"]

FROM mcr.microsoft.com/playwright:v1.64.0-noble AS tests-ihm
WORKDIR /tests
RUN mkdir /resultats && ln -s /resultats /tests/resultats
COPY tests/systeme/package.json tests/systeme/package-lock.json ./
RUN npm ci --no-audit --no-fund --loglevel=error
COPY tests/systeme/playwright.config.ts ./
COPY tests/systeme/ihm ihm
ENV CI=1
CMD ["npx", "playwright", "test"]

FROM grafana/k6:2.3.0 AS charge
USER root
RUN mkdir -p /resultats && chown 12345 /resultats
USER 12345
COPY tests/systeme/charge/charge-coups.js /scripts/charge-coups.js
CMD ["run", "/scripts/charge-coups.js", "--summary-export", "/resultats/k6-resume.json"]

# --- Étape 7 : image d'exécution minimale (sans compilateur) --------------------
# DERNIÈRE étape = cible par défaut de « docker build » (les étapes inutiles sont ignorées)
FROM ubuntu:24.04 AS execution
WORKDIR /app
COPY --from=release /src/build/linux-gcc-release/src/puissance4_serveur /app/puissance4_serveur
COPY web /app/web
EXPOSE 8080
CMD ["/app/puissance4_serveur", "--port", "8080", "--web", "/app/web"]
