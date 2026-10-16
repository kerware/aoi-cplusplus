# Dernière génération/modification faite par l'IA Claude le 13/10/2026 19:38:45
# ---------------------------------------------------------------------------
# Puissance 4 — Projet fil rouge de la formation AOI
# Itération 5 : intégration continue (base : itération 4 + corrigé du TP 4)
# Compilation multi-plateforme : Windows 11 (MSVC) et Linux (GCC, via Docker)
# ---------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
project(puissance4
        VERSION 1.4.0
        DESCRIPTION "Serveur Puissance 4 REST en C++ - architecture hexagonale"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(P4_TESTS "Construire les tests GoogleTest" ON)

# EX-21 : identification du build. La CI exporte P4_COMMIT et P4_BUILD avant la configuration
# (GitHub Actions : GITHUB_SHA / GITHUB_RUN_NUMBER ; Jenkins : GIT_COMMIT / BUILD_NUMBER).
set(P4_COMMIT "$ENV{P4_COMMIT}" CACHE STRING "Commit Git compilé (renseigné par la CI)")
set(P4_BUILD "$ENV{P4_BUILD}" CACHE STRING "Numéro de build de la CI")
if(NOT P4_COMMIT)
    set(P4_COMMIT "local")
endif()
if(NOT P4_BUILD)
    set(P4_BUILD "0")
endif()

# --- Options de compilation communes aux deux plates-formes -----------------
add_library(p4_options INTERFACE)
if(MSVC)
    # /utf-8 : sources en UTF-8 (accents) ; /permissive- : conformité stricte
    target_compile_options(p4_options INTERFACE /W4 /utf-8 /permissive- /EHsc)
    target_compile_definitions(p4_options INTERFACE NOMINMAX WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0A00)
else()
    target_compile_options(p4_options INTERFACE -Wall -Wextra -Wpedantic)
endif()

# --- Dépendances externes (téléchargées à la configuration) ------------------
# Itération 5 (CI) : archives figées par leur empreinte SHA-256 plutôt que « git clone ».
#   - reproductible : une archive modifiée ou corrompue fait échouer la configuration ;
#   - robuste : pas d'authentification Git, pas de limitation des clonages anonymes en CI ;
#   - rapide : un seul téléchargement, sans historique.
include(FetchContent)
set(FETCHCONTENT_QUIET ON)

FetchContent_Declare(httplib
    URL      https://github.com/yhirose/cpp-httplib/archive/refs/tags/v0.15.3.tar.gz
    URL_HASH SHA256=2121bbf38871bb2aafb5f7f2b9b94705366170909f434428352187cb0216124e
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)

FetchContent_Declare(json
    URL      https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    URL_HASH SHA256=d6c65aca6b1ed68e7a182f4757257b107ae403032760ed6ef121c9d55e81757d
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)

FetchContent_MakeAvailable(httplib json)

# --- Code de production ------------------------------------------------------
add_subdirectory(src)

# --- Tests -------------------------------------------------------------------
if(P4_TESTS)
    FetchContent_Declare(googletest
        URL      https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
        URL_HASH SHA256=7b42b4d6ed48810c5362c265a17faebe90dc2373c885e5216439d37927f02926
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    # Windows : même runtime (/MD) que le reste du projet
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    add_subdirectory(tests)
endif()
