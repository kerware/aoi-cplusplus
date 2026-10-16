# Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:23:27
# ---------------------------------------------------------------------------
# Client bureau Qt du Puissance 4 (EX-20) — projet CMake séparé du serveur :
# il n'utilise que l'API REST, comme l'IHM web. Tests : QtTest.
#   Windows : cmake --workflow --preset windows-mingw-qt   (Qt 6 MinGW installé dans C:/Qt)
#   Linux   : cmake --workflow --preset linux-qt           (qt6-base-dev, voir Dockerfile)
# ---------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
project(puissance4_qt VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets Network Test)
qt_standard_project_setup()

# Bibliothèque : tout sauf main(), pour être testée
qt_add_library(p4_qt STATIC
    src/PartieVue.cpp
    src/ClientApi.cpp
    src/FenetrePrincipale.cpp)
target_include_directories(p4_qt PUBLIC src)
target_link_libraries(p4_qt PUBLIC Qt6::Widgets Qt6::Network)

qt_add_executable(puissance4_qt src/main.cpp)
target_link_libraries(puissance4_qt PRIVATE p4_qt)
set_target_properties(puissance4_qt PROPERTIES WIN32_EXECUTABLE ON)

# --- Tests QtTest ------------------------------------------------------------
enable_testing()

qt_add_executable(tst_partievue tests/tst_partievue.cpp)
target_link_libraries(tst_partievue PRIVATE p4_qt Qt6::Test)
add_test(NAME tst_partievue COMMAND tst_partievue -o -,txt -o tst_partievue-junit.xml,junitxml)
set_tests_properties(tst_partievue PROPERTIES LABELS qt-unitaire)

qt_add_executable(tst_fenetre tests/tst_fenetre.cpp)
target_link_libraries(tst_fenetre PRIVATE p4_qt Qt6::Test)
add_test(NAME tst_fenetre COMMAND tst_fenetre -o -,txt -o tst_fenetre-junit.xml,junitxml)
# qt_standard_project_setup() crée des exécutables Windows « fenêtrés » (sans console) :
# les tests doivent rester des programmes console pour que CTest lise leur sortie.
set_target_properties(tst_partievue tst_fenetre PROPERTIES WIN32_EXECUTABLE OFF)
# Test d'IHM système : pilote la vraie fenêtre contre le serveur désigné par P4_URL.
# QT_QPA_PLATFORM=offscreen : aucune fenêtre affichée (CI, Docker sans écran).
set(ENV_FENETRE "QT_QPA_PLATFORM=offscreen")
if(WIN32)
    # le greffon offscreen ne voit pas les polices du système : on lui indique celles de Windows
    list(APPEND ENV_FENETRE "QT_QPA_FONTDIR=$ENV{WINDIR}/Fonts")
endif()
set_tests_properties(tst_fenetre PROPERTIES LABELS qt-systeme
    ENVIRONMENT "${ENV_FENETRE}"
    SKIP_RETURN_CODE 77)
