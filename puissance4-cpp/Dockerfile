# Dernière génération/modification faite par l'IA Claude le 09/10/2026 17:49:56
# ---------------------------------------------------------------------------
# Compilation et tests Linux du projet Puissance 4, depuis Windows + Docker Desktop
#
#   docker build --target outils -t p4-outils .        # image de la chaîne de compilation
#   docker build -t puissance4:iteration4 .            # compile + teste + image d'exécution
#   docker run --rm -p 8080:8080 puissance4:iteration4 # lance le serveur
#   docker build --target rapports --output type=local,dest=rapports .
#                                                      # couverture gcov + cppcheck + clang-tidy -> ./rapports
# ---------------------------------------------------------------------------

# --- Étape 1 : chaîne de compilation Linux (GCC 13, CMake 3.28, Ninja) ----------
FROM ubuntu:24.04 AS outils
RUN apt-get update \
 && apt-get install -y --no-install-recommends \
        build-essential cmake ninja-build git ca-certificates \
 && rm -rf /var/lib/apt/lists/*
WORKDIR /src

# --- Étape 2 : compilation + exécution des tests (le build échoue si un test échoue)
FROM outils AS build
COPY . .
RUN cmake --workflow --preset linux-gcc

# --- Étape 2 bis : compilation Release du serveur, APRÈS les tests ---------------
# Les mesures de performance (k6, Gatling) se font sur un binaire optimisé, jamais en Debug.
FROM build AS release
RUN cmake --preset linux-gcc-release && cmake --build --preset linux-gcc-release --target puissance4_serveur

# --- Étape 3 : qualité — couverture (gcov/gcovr) et analyse statique ------------
FROM outils AS qualite
RUN apt-get update \
 && apt-get install -y --no-install-recommends gcovr cppcheck clang-tidy \
 && rm -rf /var/lib/apt/lists/*
COPY . .
# Compilation instrumentée (--coverage) puis exécution des tests : produit les .gcda
RUN cmake --workflow --preset linux-gcc-couverture
# Rapports de couverture (main.cpp exclu : il n'est pas exécuté par les tests)
RUN mkdir -p /rapports/couverture \
 && gcovr --root . --filter src/ --exclude src/main.cpp --decisions \
          --html-details /rapports/couverture/index.html \
          --cobertura /rapports/couverture-cobertura.xml \
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

# --- Étape 3 bis : client Qt (Qt 6.4 d'Ubuntu) et ses tests QtTest -----------------
# Les tests unitaires Qt s'exécutent ici ; le test d'IHM système (tst_fenetre) est lancé par
# « docker compose run --rm tests-qt », contre le conteneur du serveur.
FROM outils AS qt
RUN apt-get update  && apt-get install -y --no-install-recommends qt6-base-dev libgl-dev fonts-dejavu-core  && rm -rf /var/lib/apt/lists/*
COPY qt-client /src/qt-client
WORKDIR /src/qt-client
RUN cmake --preset linux-qt && cmake --build --preset linux-qt && ctest --preset linux-qt -L qt-unitaire

# --- Étape 4 : export des rapports vers le poste (docker build --output) ---------
FROM scratch AS rapports
COPY --from=qualite /rapports /

# --- Étape 5 : image d'exécution minimale (sans compilateur) --------------------
# DERNIÈRE étape = cible par défaut de « docker build » (les étapes inutiles sont ignorées)
FROM ubuntu:24.04 AS execution
WORKDIR /app
COPY --from=release /src/build/linux-gcc-release/src/puissance4_serveur /app/puissance4_serveur
COPY web /app/web
EXPOSE 8080
CMD ["/app/puissance4_serveur", "--port", "8080", "--web", "/app/web"]
