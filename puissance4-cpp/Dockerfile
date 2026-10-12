# Dernière génération/modification faite par l'IA Claude le 09/10/2026 17:00:45
# ---------------------------------------------------------------------------
# Compilation et tests Linux du projet Puissance 4, depuis Windows + Docker Desktop
#
#   docker build --target outils -t p4-outils .        # image de la chaîne de compilation
#   docker build -t puissance4:iteration3 .            # compile + teste + image d'exécution
#   docker run --rm -p 8080:8080 puissance4:iteration3 # lance le serveur
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

# --- Étape 4 : export des rapports vers le poste (docker build --output) ---------
FROM scratch AS rapports
COPY --from=qualite /rapports /

# --- Étape 5 : image d'exécution minimale (sans compilateur) --------------------
# DERNIÈRE étape = cible par défaut de « docker build » (les étapes inutiles sont ignorées)
FROM ubuntu:24.04 AS execution
WORKDIR /app
COPY --from=build /src/build/linux-gcc/src/puissance4_serveur /app/puissance4_serveur
COPY web /app/web
EXPOSE 8080
CMD ["/app/puissance4_serveur", "--port", "8080", "--web", "/app/web"]
