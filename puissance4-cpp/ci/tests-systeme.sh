#!/bin/sh
# Dernière génération/modification faite par l'IA Claude le 13/10/2026 19:50:00
# ---------------------------------------------------------------------------
# Tests système en CI : déploie le serveur (Docker Compose), lance les injecteurs,
# rapatrie leurs résultats (JUnit, rapport Playwright, journal du serveur), nettoie TOUJOURS.
#
#   sh ci/tests-systeme.sh [dossier_resultats] [injecteurs...]
#   sh ci/tests-systeme.sh resultats/systeme                       # tests-cpp tests-api tests-ihm
#   sh ci/tests-systeme.sh resultats/charge charge                 # test de charge k6 (EX-13)
#
# Variables : COMPOSE_PROJECT_NAME (un nom par build, défaut p4-ci), P4_COMMIT, P4_BUILD (EX-21).
# Code de retour : 0 si tous les injecteurs ont réussi, 1 sinon (les résultats sont rapatriés quand même).
# ---------------------------------------------------------------------------
set -u
RESULTATS=${1:-resultats/systeme}
[ $# -gt 0 ] && shift
INJECTEURS=${*:-tests-cpp tests-api tests-ihm}
PROJET=${COMPOSE_PROJECT_NAME:-p4-ci}
DC="docker compose -f docker-compose.ci.yml -p $PROJET --profile tests --profile charge"

mkdir -p "$RESULTATS"
nettoyer() {
    $DC logs --no-color serveur > "$RESULTATS/serveur.log" 2>&1
    $DC down --volumes --remove-orphans --rmi local > /dev/null 2>&1   # images du build supprimées (le cache reste)
}
trap nettoyer EXIT

echo "== Construction des images (serveur + injecteurs : $INJECTEURS)"
$DC build serveur $INJECTEURS || exit 1
echo "== Déploiement du serveur"
$DC up -d --wait serveur || exit 1

code=0
for injecteur in $INJECTEURS; do
    conteneur="$PROJET-$injecteur"
    echo "== $injecteur"
    if $DC run --name "$conteneur" "$injecteur"; then
        echo "== $injecteur : OK"
    else
        echo "== $injecteur : ÉCHEC"
        code=1
    fi
    docker cp "$conteneur:/resultats/." "$RESULTATS/" > /dev/null 2>&1
    docker rm "$conteneur" > /dev/null 2>&1
done
exit $code
