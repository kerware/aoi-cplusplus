# Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:13:12
"""Publie des résultats JUnit dans une itération Squash TM (API REST « import des résultats »).

  POST {SQUASH_TM_URL}/api/rest/latest/import/results/{SQUASH_TM_ITERATION_ID}
  Authorization: Bearer {SQUASH_TM_API_TOKEN}            (jeton API personnel, à créer dans Squash TM)

Chaque cas de test JUnit devient un résultat identifié par sa RÉFÉRENCE : elle doit être recopiée
dans le champ « Référence du test automatisé » du cas de test Squash TM correspondant, et ce cas
doit figurer dans le plan d'exécution de l'itération. Squash TM crée alors une « suite automatisée ».

  python3 ci/publier_squash.py resultats/*.xml --liste            # affiche les références trouvées
  python3 ci/publier_squash.py resultats/*.xml --simulation       # écrit squash-resultats.json, n'envoie rien
  python3 ci/publier_squash.py resultats/*.xml --inclure "^ApiSystemeTest\\."   # publie (variables ci-dessus)
  python3 ci/publier_squash.py resultats/*.xml --grouper "/"   # Bruno : une référence par requête

Sans SQUASH_TM_URL / SQUASH_TM_API_TOKEN / SQUASH_TM_ITERATION_ID, la publication est ignorée (code 0) :
le pipeline reste utilisable sans Squash TM. Bibliothèque standard Python uniquement.
"""
import argparse
import base64
import glob
import json
import os
import re
import sys
import urllib.error
import urllib.request
import xml.etree.ElementTree as ET

# Du moins grave au plus grave : quand une référence apparaît plusieurs fois (ex. Playwright sur
# Chromium ET Firefox), le résultat publié est le plus grave.
GRAVITE = ["SUCCESS", "SKIPPED", "BLOCKED", "FAILURE"]


def reference(cas, grouper=None):
    """GoogleTest/CTest : « Suite.test » ; autres outils : « classe.nom ».
    Si la classe correspond à l'expression « grouper », la référence est la classe seule : tous ses cas
    sont regroupés (ex. Bruno produit un cas par assertion, Squash TM attend un résultat par requête)."""
    nom = cas.get("name", "").strip()
    classe = (cas.get("classname") or "").strip()
    if grouper and classe and re.search(grouper, classe):
        return classe
    return nom if not classe or classe == nom else f"{classe}.{nom}"


def statut(cas):
    if cas.find("failure") is not None or cas.find("error") is not None:
        return "FAILURE"
    if cas.find("skipped") is not None or cas.get("status") == "notrun":
        return "SKIPPED"
    return "SUCCESS"


def details(cas):
    messages = []
    for balise in ("failure", "error"):
        for el in cas.findall(balise):
            texte = (el.get("message") or "") + ("\n" + el.text.strip() if el.text and el.text.strip() else "")
            messages.append(texte.strip()[:2000])
    return messages


def lire_junit(fichiers, grouper=None):
    resultats = {}
    for fichier in fichiers:
        for cas in ET.parse(fichier).getroot().iter("testcase"):
            ref = reference(cas, grouper)
            st = statut(cas)
            duree = int(float(cas.get("time") or 0) * 1000)
            if ref in resultats:
                r = resultats[ref]
                r["status"] = max(r["status"], st, key=GRAVITE.index)
                r["duration"] += duree
                r.setdefault("failure_details", []).extend(details(cas))
            else:
                resultats[ref] = {"reference": ref, "status": st, "duration": duree, "failure_details": details(cas)}
    for r in resultats.values():
        if not r["failure_details"]:
            del r["failure_details"]
    return list(resultats.values())


def piece_jointe(chemin):
    with open(chemin, "rb") as f:
        return {"name": os.path.basename(chemin), "content": base64.b64encode(f.read()).decode("ascii")}


def main():
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")  # console Windows
    p = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    p.add_argument("junit", nargs="+", help="fichiers JUnit XML (motifs acceptés)")
    p.add_argument("--inclure", help="expression régulière : ne publier que les références qui y correspondent")
    p.add_argument("--grouper", help="expression régulière sur la classe JUnit : un résultat par classe")
    p.add_argument("--piece-jointe", action="append", default=[], help="fichier joint à la suite (txt, html, xml)")
    p.add_argument("--sortie", default="squash-resultats.json", help="corps JSON écrit sur le disque")
    p.add_argument("--liste", action="store_true", help="afficher les références et sortir")
    p.add_argument("--simulation", action="store_true", help="préparer le JSON sans l'envoyer")
    args = p.parse_args()

    fichiers = sorted({f for motif in args.junit for f in glob.glob(motif, recursive=True)})
    if not fichiers:
        print("Aucun fichier JUnit trouvé :", " ".join(args.junit))
        return 1
    tests = lire_junit(fichiers, args.grouper)
    if args.inclure:
        tests = [t for t in tests if re.search(args.inclure, t["reference"])]
    if args.liste:
        for t in tests:
            print(f"{t['status']:8} {t['reference']}")
        return 0

    corps = {"tests": tests}
    if args.piece_jointe:
        corps["automated_test_suite"] = {"attachments": [piece_jointe(c) for c in args.piece_jointe]}
    with open(args.sortie, "w", encoding="utf8") as f:
        json.dump(corps, f, ensure_ascii=False, indent=2)
    bilan = {s: sum(1 for t in tests if t["status"] == s) for s in GRAVITE}
    print(f"{len(tests)} résultat(s) préparé(s) dans {args.sortie} : {bilan}")

    url, jeton, iteration = (os.environ.get(v, "").strip() for v in
                             ("SQUASH_TM_URL", "SQUASH_TM_API_TOKEN", "SQUASH_TM_ITERATION_ID"))
    if args.simulation or not (url and jeton and iteration):
        print("Publication Squash TM ignorée (simulation, ou SQUASH_TM_URL / SQUASH_TM_API_TOKEN / "
              "SQUASH_TM_ITERATION_ID non définis).")
        return 0

    adresse = f"{url.rstrip('/')}/api/rest/latest/import/results/{int(iteration)}"
    requete = urllib.request.Request(adresse, data=json.dumps(corps).encode("utf8"), method="POST",
                                     headers={"Content-Type": "application/json",
                                              "Authorization": f"Bearer {jeton}"})
    try:
        with urllib.request.urlopen(requete, timeout=60) as reponse:
            code, texte = reponse.status, reponse.read().decode("utf8", "replace")
    except urllib.error.HTTPError as erreur:
        code, texte = erreur.code, erreur.read().decode("utf8", "replace")
    except urllib.error.URLError as erreur:
        print(f"Squash TM injoignable ({adresse}) : {erreur.reason}")
        return 1
    if code == 204:
        print(f"Résultats publiés dans l'itération {iteration} de Squash TM.")
        return 0
    if code == 207:
        print(f"Publication PARTIELLE (207) : références absentes du plan d'exécution ?\n{texte[:2000]}")
        return 0
    print(f"Échec de la publication : HTTP {code}\n{texte[:2000]}")
    return 1


if __name__ == "__main__":
    sys.exit(main())
