# Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:23:27
# ---------------------------------------------------------------------------
# Trois exécutables de test, trois niveaux de la pyramide :
#   tests_unitaires   : domaine + application, ports de sortie bouchonnés (GoogleMock)
#                       -> ne dépend QUE de p4_domaine (rapide, aucun réseau)
#   tests_integration : adaptateurs réels (mémoire, serveur HTTP sur un port libre)
#   tests_systeme     : API d'un serveur DÉJÀ déployé (variable P4_URL), sinon ignorés
# Chaque test GoogleTest devient un test CTest étiqueté (LABELS) :
#   ctest --preset <preset> -L unitaire | -L integration | -L systeme
# ---------------------------------------------------------------------------
include(GoogleTest)

add_executable(tests_unitaires
    unitaires/GrilleTest.cpp
    unitaires/JoueurTest.cpp
    unitaires/PartieTest.cpp
    unitaires/ServicePartieTest.cpp)
target_link_libraries(tests_unitaires PRIVATE p4_domaine p4_options GTest::gmock_main)
gtest_discover_tests(tests_unitaires
    PROPERTIES LABELS unitaire
    DISCOVERY_TIMEOUT 30)

add_executable(tests_integration
    integration/PartieRepositoryEnMemoireTest.cpp
    integration/GenerateurIdentifiantSequentielTest.cpp
    integration/ServeurRestTest.cpp)
target_link_libraries(tests_integration PRIVATE p4_adaptateurs p4_options GTest::gtest_main)
target_compile_definitions(tests_integration PRIVATE
    P4_DOSSIER_WEB="${PROJECT_SOURCE_DIR}/web")
gtest_discover_tests(tests_integration
    PROPERTIES LABELS integration
    DISCOVERY_TIMEOUT 30)

# Tests système : client HTTP seul (aucune dépendance au code de production)
add_executable(tests_systeme systeme/ApiSystemeTest.cpp)
target_link_libraries(tests_systeme PRIVATE httplib::httplib nlohmann_json::nlohmann_json p4_options GTest::gtest_main)
# std::getenv est portable ; MSVC le signale (C4996) en proposant _dupenv_s, propre à Windows
target_compile_definitions(tests_systeme PRIVATE _CRT_SECURE_NO_WARNINGS)
gtest_discover_tests(tests_systeme
    PROPERTIES LABELS systeme
               SKIP_REGULAR_EXPRESSION "\\[  SKIPPED \\]"
    DISCOVERY_TIMEOUT 30)
