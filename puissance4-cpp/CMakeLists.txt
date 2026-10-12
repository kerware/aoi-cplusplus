# Dernière génération/modification faite par l'IA Claude le 12/10/2026 23:09:26
# ---------------------------------------------------------------------------
# Puissance 4 — Projet fil rouge de la formation AOI
# Itération 4 : tests système et client Qt (base : itération 3 + corrigé du TP 3)
# Compilation multi-plateforme : Windows 11 (MSVC) et Linux (GCC, via Docker)
# ---------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
project(puissance4
        VERSION 1.3.0
        DESCRIPTION "Serveur Puissance 4 REST en C++ - architecture hexagonale"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(P4_TESTS "Construire les tests GoogleTest" ON)

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
include(FetchContent)
set(FETCHCONTENT_QUIET ON)

FetchContent_Declare(httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG        v0.15.3
    GIT_SHALLOW    TRUE)

FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
    GIT_SHALLOW    TRUE)

FetchContent_MakeAvailable(httplib json)

# --- Code de production ------------------------------------------------------
add_subdirectory(src)

# --- Tests -------------------------------------------------------------------
if(P4_TESTS)
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.15.2
        GIT_SHALLOW    TRUE)
    # Windows : même runtime (/MD) que le reste du projet
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    add_subdirectory(tests)
endif()
