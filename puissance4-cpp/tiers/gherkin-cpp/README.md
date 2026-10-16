<!-- Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:58:24 -->
# gherkin-cpp — du Gherkin au code C++ exécutable

Lire des scénarios Gherkin (`.feature`), **générer le code glue** C++ (les définitions d'étapes), le
compléter, recompiler, puis **exécuter les scénarios** : chaque scénario devient un test GoogleTest, donc un
test CTest, avec rapport JUnit pour l'intégration continue.

Projet pédagogique de la formation AOI : C++20, CMake, GoogleTest, aucune autre dépendance (ni Ruby, ni
serveur « wire »). Compilé et testé sous Windows 11 (MSVC 19.44) et Linux (GCC 13.3, Docker).

## Le cycle de travail

```
 calculatrice.feature ──gherkin-glue──► etapes_generees.cpp ──vous complétez──► compilation ──► scénarios exécutés
   (Gherkin, français     (squelette :        (appel au code testé,                 (1 scénario = 1 test
    ou anglais)           cuke::enAttente)     EXPECT_…)                              GoogleTest / CTest)
```

| Étape | Commande (Windows ; Linux : preset `linux-gcc`) | Résultat |
|---|---|---|
| 0. Construire l'outil | `cmake --preset windows-msvc` puis `cmake --build --preset windows-msvc` | `gherkin-glue`, bibliothèques |
| 1. Écrire le Gherkin | `exemple/features/calculatrice.feature` | |
| 2. Générer le code glue | `cmake --build --preset windows-msvc --target calculatrice_bdd_glue` | `exemple/etapes/etapes_generees.cpp` |
| 3. Recompiler, exécuter | `cmake --build --preset windows-msvc` puis `ctest --preset windows-msvc -L bdd` | scénarios **ignorés** (en attente) |
| 4. Compléter le code glue | remplacer chaque `cuke::enAttente()` par le code (voir `exemple/etapes/etapes_calculatrice.cpp`) | |
| 5. Recompiler, exécuter | idem 3 | scénarios **réussis** ou **en échec** |
| 6. Nouvelles étapes | `cmake --build --preset windows-msvc --target calculatrice_bdd_extraits` | code glue des seules étapes manquantes |

Le générateur n'écrase **jamais** un fichier existant (c'est votre code) sauf avec `--forcer`. Les fichiers
du dossier `etapes/` sont repris automatiquement à la compilation suivante.

## Exemple

```gherkin
# language: fr
Fonctionnalité: Calculatrice
  Contexte:
    Soit une calculatrice neuve

  Scénario: Additionner deux nombres
    Quand j'additionne 2 et 3
    Alors le résultat est 5

  Plan du scénario: Multiplier deux nombres
    Quand je multiplie <a> par <b>
    Alors le résultat est <produit>
    Exemples:
      | a  | b | produit |
      | 2  | 3 | 6       |
      | -4 | 5 | -20     |
```

Code glue **généré** (étape 2) :

```cpp
// exemple/features/calculatrice.feature:13 — Quand j'additionne 2 et 3
QUAND("j'additionne {int} et {int}") {
    [[maybe_unused]] const int entier1 = arg.entier(0);
    [[maybe_unused]] const int entier2 = arg.entier(1);
    cuke::enAttente();   // À FAIRE : appeler le code à tester, puis vérifier (EXPECT_…)
}
```

Code glue **complété** (étape 4) :

```cpp
struct Monde { calc::Calculatrice calculatrice; double resultat = 0; };   // état d'un scénario

QUAND("j'additionne {int} et {int}") {
    auto& m = ctx.obtenir<Monde>();
    m.resultat = m.calculatrice.additionner(arg.entier(0), arg.entier(1));
}

ALORS("le résultat est {int}") {
    EXPECT_DOUBLE_EQ(ctx.obtenir<Monde>().resultat, arg.entier(0));
}
```

Exécution (étape 5) :

```
[ RUN      ] Calculatrice.Additionner_deux_nombres
    Scénario : Additionner deux nombres  (exemple/features/calculatrice.feature:12)
      Soit une calculatrice neuve  [ok]
      Quand j'additionne 2 et 3  [ok]
      Alors le résultat est 5  [ok]
[       OK ] Calculatrice.Additionner_deux_nombres (0 ms)
```

## Ce que reconnaît l'analyseur

| Élément | Français | Anglais |
|---|---|---|
| Fonctionnalité | `Fonctionnalité:` | `Feature:` |
| Contexte (avant chaque scénario) | `Contexte:` | `Background:` |
| Scénario | `Scénario:`, `Exemple:` | `Scenario:`, `Example:` |
| Plan du scénario + exemples | `Plan du scénario:` + `Exemples:` | `Scenario Outline:` + `Examples:` |
| Étapes | `Soit`, `Étant donné`, `Quand`, `Lorsque`, `Alors`, `Donc`, `Et`, `Mais`, `*` | `Given`, `When`, `Then`, `And`, `But`, `*` |
| Règle (regroupement) | `Règle:` | `Rule:` |

Aussi : tags (`@rapide`), commentaires, tables de données (`| a | b |`), doc strings (`"""` ou `` ``` ``).
La langue se choisit par `# language: fr` en tête du fichier (anglais par défaut).

Paramètres des étapes (sous-ensemble des Cucumber Expressions) : `{int}`, `{float}` (`3.5` ou `3,5`),
`{string}` (entre guillemets droits), `{word}`, `{}`. Le générateur les déduit du texte : nombres →
`{int}` / `{float}`, texte entre guillemets → `{string}`, `<paramètre>` d'un plan → type déduit des Exemples.

## Écrire le code glue

| Dans une étape | Rôle |
|---|---|
| `SOIT("…")`, `QUAND("…")`, `ALORS("…")`, `ETAPE("…")` (ou `GIVEN`, `WHEN`, `THEN`, `STEP`) | définir une étape ; le mot-clé est indicatif, seul le texte compte |
| `arg.entier(i)`, `arg.reel(i)`, `arg.texte(i)` | paramètres capturés |
| `arg.table()` → `colonne("nom")`, `enDictionnaires()` | table de données de l'étape |
| `arg.docString()` | doc string de l'étape |
| `ctx.obtenir<Monde>()` | état partagé par les étapes d'**un** scénario (objet neuf pour chaque scénario) |
| `EXPECT_…`, `ASSERT_…` | vérifications GoogleTest |
| `cuke::enAttente()` | étape pas encore implémentée : scénario **ignoré** (SKIPPED) |

Règles d'exécution (comme Cucumber) : une étape sans définition ou avec plusieurs définitions fait échouer le
scénario (le message donne le code glue à ajouter) ; après une étape en échec, les suivantes sont sautées.

## Options de l'exécutable de scénarios

```powershell
.\build\windows-msvc\exemple\Debug\calculatrice_bdd.exe                         # tous les scénarios
.\build\windows-msvc\exemple\Debug\calculatrice_bdd.exe --tags "@erreur or @plan"
.\build\windows-msvc\exemple\Debug\calculatrice_bdd.exe autre\dossier\features   # d'autres .feature
.\build\windows-msvc\exemple\Debug\calculatrice_bdd.exe --extraits              # étapes non définies
.\build\windows-msvc\exemple\Debug\calculatrice_bdd.exe --gtest_output=xml:bdd.xml   # rapport JUnit
```

Dans votre projet : `include(cmake/Gherkin.cmake)` puis

```cmake
gherkin_ajouter_executable(mon_projet_bdd
    FEATURES "${CMAKE_CURRENT_SOURCE_DIR}/features"
    ETAPES   "${CMAKE_CURRENT_SOURCE_DIR}/etapes"
    LIENS    ma_bibliotheque)
```

## Organisation

| Dossier | Contenu |
|---|---|
| `bibliotheque/include/gherkin`, `src` | analyseur (`Analyseur`, `Ast`), expressions d'étapes, générateur de code glue (`Glue`), filtre de tags |
| `bibliotheque/include/cuke`, `src/Cuke.cpp` | exécution : registre des étapes, macros, un test GoogleTest par scénario |
| `outils/gherkin_glue.cpp` | l'outil `gherkin-glue` |
| `cmake/Gherkin.cmake` | `gherkin_ajouter_executable()` et les cibles `_glue`, `_extraits` |
| `exemple/` | calculatrice : `features/`, `etapes/` (code glue complété), `src/` (code testé) |
| `tests/` | 32 tests unitaires + 2 tests de bout en bout |

## L'existant (octobre 2026)

| Projet | Principe | Dépendances | Remarques |
|---|---|---|---|
| [cucumber-cpp](https://github.com/cucumber/cucumber-cpp) (v0.9.0) | définitions d'étapes en C++, exécutées par Cucumber **Ruby** via le protocole « wire » | Ruby, gem `cucumber-wire`, Boost (selon version) | projet officiel ; architecture lourde, compatibilité délicate entre versions |
| [cwt-cucumber](https://github.com/ThoSe1990/cwt-cucumber) (2.10) | interpréteur Cucumber natif C++20 | aucune obligatoire | macros `GIVEN/WHEN/THEN`, Cucumber Expressions, tables, hooks ; MIT ; sur Conan Center |
| [amp-cucumber-cpp-runner](https://github.com/philips-software/amp-cucumber-cpp-runner) (v4.3.0) | exécuteur natif sur l'analyseur Gherkin officiel | analyseur `cucumber/gherkin` | orienté embarqué (Philips) |
| [GUnit](https://github.com/cpp-testing/GUnit) (v1.16.0) | extension de GoogleTest / GoogleMock, avec scénarios Gherkin (GSteps) | GoogleTest | macros propres à GUnit |
| [cucumber/gherkin](https://github.com/cucumber/gherkin) | analyseur Gherkin officiel (dont une version C++) | — | analyse seulement, pas d'exécution |
| Cornichon | génère des tests GoogleTest à partir du Gherkin | Python | génération statique |
| Catch2 (`SCENARIO`, `GIVEN`…) | style BDD dans le code C++ | Catch2 | pas de fichiers `.feature` |

**Pourquoi ce projet ?** Montrer en formation, sur peu de code (≈ 1 700 lignes, commentaires compris), les trois mécanismes d'un
outil BDD : analyse du Gherkin, correspondance texte → définition d'étape, génération du code glue ; et
s'intégrer à la chaîne déjà utilisée (CMake, GoogleTest, CTest, JUnit). **En production**, préférer un outil
maintenu : cwt-cucumber (natif, sans dépendance) ou cucumber-cpp (écosystème Cucumber officiel).

**Limites connues** : Cucumber Expressions partielles (pas de texte optionnel `(s)`, d'alternatives `a/b` ni de
types personnalisés), pas de hooks (`Before`/`After`), une seule `Fonctionnalité` par fichier, `Règle` sans
contexte propre, langues : français et anglais.
