# Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:00:00
"""Seuil de qualité (« quality gate ») commun à GitHub Actions et Jenkins.

Lit les rapports produits par « docker build --target rapports » et ÉCHOUE (code 1) si :
  - la couverture des lignes est inférieure au seuil (défaut 90 %) ;
  - cppcheck ou clang-tidy signalent au moins une remarque (défaut : 0 tolérée).
Écrit un résumé Markdown (sortie standard + fichier $GITHUB_STEP_SUMMARY s'il existe).

  python3 ci/seuil_qualite.py rapports [--lignes 90] [--remarques 0]
"""
import argparse
import json
import os
import sys
import xml.etree.ElementTree as ET


def lire_remarques_cppcheck(chemin):
    if not os.path.exists(chemin):
        return None
    return len(ET.parse(chemin).getroot().findall(".//error"))


def lire_remarques_clang_tidy(chemin):
    if not os.path.exists(chemin):
        return None
    with open(chemin, encoding="utf8", errors="replace") as f:
        return sum(1 for ligne in f if ": warning:" in ligne or ": error:" in ligne)


def main():
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")  # console Windows
    parametres = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parametres.add_argument("rapports", help="dossier des rapports (couverture-resume.json, cppcheck.xml…)")
    parametres.add_argument("--lignes", type=float, default=90.0, help="couverture minimale des lignes (%%)")
    parametres.add_argument("--remarques", type=int, default=0, help="remarques d'analyse statique tolérées")
    args = parametres.parse_args()

    with open(os.path.join(args.rapports, "couverture-resume.json"), encoding="utf8") as f:
        resume = json.load(f)
    lignes = resume["line_percent"]
    cppcheck = lire_remarques_cppcheck(os.path.join(args.rapports, "cppcheck.xml"))
    clang_tidy = lire_remarques_clang_tidy(os.path.join(args.rapports, "clang-tidy.txt"))

    criteres = [
        ("Couverture des lignes", f"{lignes:.1f} %", f"≥ {args.lignes:g} %", lignes >= args.lignes),
        ("Couverture des fonctions", f"{resume['function_percent']:.1f} %", "(information)", True),
        ("Remarques cppcheck", str(cppcheck), f"≤ {args.remarques}", cppcheck is not None and cppcheck <= args.remarques),
        ("Remarques clang-tidy", str(clang_tidy), f"≤ {args.remarques}",
         clang_tidy is not None and clang_tidy <= args.remarques),
    ]
    reussi = all(ok for *_, ok in criteres)
    texte = ["## Seuil de qualité : " + ("OK, franchi" if reussi else "KO, NON franchi"), "",
             "| Critère | Mesure | Attendu | Résultat |", "|---|---|---|---|"]
    texte += [f"| {nom} | {mesure} | {attendu} | {'OK' if ok else '**KO**'} |" for nom, mesure, attendu, ok in criteres]
    texte = "\n".join(texte) + "\n"

    sys.stdout.write(texte)
    if os.environ.get("GITHUB_STEP_SUMMARY"):
        with open(os.environ["GITHUB_STEP_SUMMARY"], "a", encoding="utf8") as f:
            f.write(texte)
    with open(os.path.join(args.rapports, "seuil-qualite.md"), "w", encoding="utf8") as f:
        f.write(texte)
    return 0 if reussi else 1


if __name__ == "__main__":
    sys.exit(main())
