# Dernière génération/modification faite par l'IA Claude le 13/10/2026 21:02:38
# ---------------------------------------------------------------------------
# gherkin-cpp : lire du Gherkin, générer le code glue C++, exécuter les scénarios avec GoogleTest / CTest
#   bibliotheque/  gherkin (analyseur, expressions, générateur de glue, tags) + cuke (exécution)
#   outils/        gherkin-glue : .feature -> squelette de code glue
#   exemple/       calculatrice : features, code glue complété, code à tester
#   tests/         tests unitaires de la bibliothèque
# ---------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
project(gherkin_cpp VERSION 0.1.0 LANGUAGES CXX)

# Utilisable seul ou comme sous-projet (add_subdirectory) : dans ce cas, GoogleTest vient du projet
# parent, et ni les tests de gherkin-cpp ni l'exemple ne sont construits.
option(GHERKIN_CPP_EXEMPLES "Construire les tests et l'exemple de gherkin-cpp" ${PROJECT_IS_TOP_LEVEL})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /utf-8 /permissive- /EHsc)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# --- GoogleTest (archive vérifiée par son empreinte), sauf s'il est déjà fourni -------
if(NOT TARGET GTest::gtest)
    include(FetchContent)
    FetchContent_Declare(googletest
        URL      https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
        URL_HASH SHA256=7b42b4d6ed48810c5362c265a17faebe90dc2373c885e5216439d37927f02926
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

enable_testing()
include(GoogleTest)

# --- Bibliothèques -------------------------------------------------------------
add_library(gherkin STATIC
    bibliotheque/src/Ast.cpp
    bibliotheque/src/Analyseur.cpp
    bibliotheque/src/Expressions.cpp
    bibliotheque/src/Glue.cpp
    bibliotheque/src/Tags.cpp)
target_include_directories(gherkin PUBLIC bibliotheque/include)

add_library(cuke STATIC bibliotheque/src/Cuke.cpp)
target_link_libraries(cuke PUBLIC gherkin GTest::gtest)

# --- Générateur de code glue ---------------------------------------------------
add_executable(gherkin-glue outils/gherkin_glue.cpp)
target_link_libraries(gherkin-glue PRIVATE gherkin)

include(cmake/Gherkin.cmake)

if(GHERKIN_CPP_EXEMPLES)
    add_subdirectory(tests)
    add_subdirectory(exemple)
endif()
