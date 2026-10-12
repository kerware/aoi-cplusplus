# Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
# ---------------------------------------------------------------------------
# Trois cibles = trois anneaux de l'hexagone
#   p4_domaine     : modèle + ports + cas d'utilisation (AUCUNE dépendance externe)
#   p4_adaptateurs : REST (cpp-httplib, JSON) et persistance en mémoire
#   puissance4_serveur : exécutable, assemble le tout (composition root)
# ---------------------------------------------------------------------------

add_library(p4_domaine STATIC
    domaine/Jeton.cpp
    domaine/Grille.cpp
    domaine/Joueur.cpp
    domaine/Partie.cpp
    application/ServicePartie.cpp)
target_include_directories(p4_domaine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(p4_domaine PRIVATE p4_options)

add_library(p4_adaptateurs STATIC
    adaptateurs/sortie/memoire/PartieRepositoryEnMemoire.cpp
    adaptateurs/sortie/memoire/GenerateurIdentifiantSequentiel.cpp
    adaptateurs/entree/rest/PartieJson.cpp
    adaptateurs/entree/rest/ServeurRest.cpp)
target_link_libraries(p4_adaptateurs
    PUBLIC  p4_domaine httplib::httplib nlohmann_json::nlohmann_json
    PRIVATE p4_options)

add_executable(puissance4_serveur main.cpp)
target_link_libraries(puissance4_serveur PRIVATE p4_adaptateurs p4_options)

# L'IHM HTML5 est copiée à côté de l'exécutable : <bin>/web/index.html
add_custom_command(TARGET puissance4_serveur POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${PROJECT_SOURCE_DIR}/web $<TARGET_FILE_DIR:puissance4_serveur>/web)
