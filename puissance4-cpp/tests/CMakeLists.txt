# Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
# ---------------------------------------------------------------------------
# Deux exécutables de test, deux niveaux de la pyramide :
#   tests_unitaires   : domaine + application, ports de sortie bouchonnés (GoogleMock)
#                       -> ne dépend QUE de p4_domaine (rapide, aucun réseau)
#   tests_integration : adaptateurs réels (mémoire, serveur HTTP sur un port libre)
# Chaque test GoogleTest devient un test CTest étiqueté (LABELS) :
#   ctest --preset <preset> -L unitaire   |   ctest --preset <preset> -L integration
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
