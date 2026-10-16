# Dernière génération/modification faite par l'IA Claude le 13/10/2026 21:06:25
# ---------------------------------------------------------------------------
# gherkin_ajouter_executable(<cible> FEATURES <dossier> ETAPES <dossier> [SOURCES …] [LIENS …])
#
# Crée l'exécutable de scénarios <cible> : main() de cuke + tous les .cpp du dossier ETAPES (code glue).
# Chaque scénario devient un test CTest (label « bdd »). Deux cibles d'aide :
#   <cible>_glue      génère ETAPES/etapes_generees.cpp à partir des .feature (si le fichier n'existe pas)
#   <cible>_extraits  affiche le code glue des étapes pas encore définies
# Les fichiers d'ETAPES sont relus à chaque compilation (CONFIGURE_DEPENDS) : un fichier généré est pris
# en compte automatiquement.
# ---------------------------------------------------------------------------
function(gherkin_ajouter_executable cible)
    cmake_parse_arguments(ARG "" "FEATURES;ETAPES" "SOURCES;LIENS" ${ARGN})
    if(NOT ARG_FEATURES OR NOT ARG_ETAPES)
        message(FATAL_ERROR "gherkin_ajouter_executable : FEATURES et ETAPES sont obligatoires")
    endif()
    # main() de cuke ; chemin calculé ici pour fonctionner aussi depuis un projet parent
    set(principal "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../bibliotheque/src/principal.cpp")
    file(GLOB etapes CONFIGURE_DEPENDS "${ARG_ETAPES}/*.cpp")
    add_executable(${cible} ${principal} ${etapes} ${ARG_SOURCES})
    target_link_libraries(${cible} PRIVATE cuke ${ARG_LIENS})
    target_compile_definitions(${cible} PRIVATE CUKE_FEATURES="${ARG_FEATURES}")

    include(GoogleTest)
    gtest_discover_tests(${cible}
        PROPERTIES LABELS bdd
                   SKIP_REGULAR_EXPRESSION "\\[  SKIPPED \\]"
        DISCOVERY_TIMEOUT 30)

    add_custom_target(${cible}_glue
        COMMAND gherkin-glue "${ARG_FEATURES}" --sortie "${ARG_ETAPES}/etapes_generees.cpp"
        # Reconfigurer tout de suite : sinon, avec le générateur Visual Studio, une compilation limitée à
        # une cible utiliserait encore l'ancien projet (sans le fichier généré).
        COMMAND "${CMAKE_COMMAND}" -S "${CMAKE_SOURCE_DIR}" -B "${CMAKE_BINARY_DIR}"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "Génération du code glue à partir de ${ARG_FEATURES}"
        VERBATIM)
    add_custom_target(${cible}_extraits
        COMMAND ${cible} --extraits
        DEPENDS ${cible}
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "Étapes non définies de ${ARG_FEATURES}"
        VERBATIM)
endfunction()
