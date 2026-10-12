# Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
# ---------------------------------------------------------------------------
# Compilation et tests Linux du projet Puissance 4, depuis Windows + Docker Desktop
#
#   docker build --target outils -t p4-outils .        # image de la chaîne de compilation
#   docker build -t puissance4:iteration1 .            # compile + teste + image d'exécution
#   docker run --rm -p 8080:8080 puissance4:iteration1 # lance le serveur
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

# --- Étape 3 : image d'exécution minimale (sans compilateur) --------------------
FROM ubuntu:24.04 AS execution
WORKDIR /app
COPY --from=build /src/build/linux-gcc/src/puissance4_serveur /app/puissance4_serveur
COPY web /app/web
EXPOSE 8080
CMD ["/app/puissance4_serveur", "--port", "8080", "--web", "/app/web"]
