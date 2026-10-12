<!-- Dernière génération/modification faite par l'IA Claude le 12/10/2026 23:37:30 -->
# AOI — Automatisation des tests d'applications C++

Projet fil rouge de la formation **AOI** : un serveur de **Puissance 4** en C++20, exposé par une API REST, construit en **architecture hexagonale** et compilé sur **Windows 11 (MSVC)** et **Linux (GCC, via Docker)**.

Le projet grandit d'une itération par chapitre de la formation. Chaque itération est publiée à la fin du chapitre correspondant, avec une étiquette Git :

| Étiquette | Chapitre | Contenu |
|---|---|---|
| `iteration-1` | 1. Démarche de tests | créer une partie : squelette multi-plate-forme (CMake, Docker), architecture hexagonale, premiers tests GoogleTest |

```bash
git clone https://github.com/kerware/aoi-cplusplus.git
git checkout iteration-1      # revenir à l'état d'une itération donnée
```

## Contenu

| Dossier | Rôle |
|---|---|
| [`installation/`](installation/) | **à faire en premier** : note d'installation Windows 11 (C++20, MSVC) et script de vérification du poste |
| [`puissance4-cpp/`](puissance4-cpp/README.md) | le projet : serveur, IHM HTML5, tests ; **mode d'emploi complet dans son README** |

## Prérequis

- Windows 11 : Visual Studio 2022 ou Build Tools 2022 (C++), CMake ≥ 3.25, Git
- Docker Desktop (compilation et tests sous Linux)

Avant la formation, suivre la [note d'installation](installation/Note-installation-Windows11-Cpp20-MSVC.docx) puis vérifier le poste :

```powershell
powershell -ExecutionPolicy Bypass -File .\installation\verifier-environnement.ps1
```

Démarrage rapide (PowerShell, dans `puissance4-cpp`) :

```powershell
cmake --workflow --preset windows-msvc     # Windows : configure, compile, teste
docker build -t puissance4 .              # Linux : compile et teste dans Docker
```

## Licence

Code publié sous licence [MIT](LICENSE) — © 2026 Kerware.
