# Dernière génération/modification faite par l'IA Claude le 12/10/2026 23:29:41
# ---------------------------------------------------------------------------
# Vérification du poste de formation AOI : C++20 sous Windows 11 avec MSVC
#   powershell -ExecutionPolicy Bypass -File .\verifier-environnement.ps1
# Code de sortie : 0 si tout ce qui est obligatoire est conforme, 1 sinon.
# Le script ne modifie rien sur le poste (sauf le dossier verification-cpp20\build).
# ---------------------------------------------------------------------------
$ErrorActionPreference = "Continue"
$script:echecs = 0
$script:alertes = 0

function Ecrire-Resultat([string]$etat, [string]$libelle, [string]$detail) {
    $couleur = @{ "OK" = "Green"; "KO" = "Red"; "!!" = "Yellow" }[$etat]
    Write-Host ("[{0}] " -f $etat) -ForegroundColor $couleur -NoNewline
    Write-Host ("{0,-38} {1}" -f $libelle, $detail)
    if ($etat -eq "KO") { $script:echecs++ }
    if ($etat -eq "!!") { $script:alertes++ }
}

Write-Host ""
Write-Host "Formation AOI - vérification de l'environnement C++20 / MSVC" -ForegroundColor Cyan
Write-Host ""

# 1. Windows 11 (build 22000 ou plus)
$os = Get-CimInstance Win32_OperatingSystem
if ([int]$os.BuildNumber -ge 22000) {
    Ecrire-Resultat "OK" "Windows 11" ("{0} (build {1})" -f $os.Caption, $os.BuildNumber)
} else {
    Ecrire-Resultat "KO" "Windows 11" ("{0} (build {1}) : Windows 11 requis" -f $os.Caption, $os.BuildNumber)
}

# 2. Visual Studio 2022 avec les outils C++ (MSVC x64)
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vs2022 = $null
if (Test-Path $vswhere) {
    $vs2022 = & $vswhere -products * -version "[17.0,18.0)" -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -latest -format json | ConvertFrom-Json
    $vs2026 = & $vswhere -products * -version "[18.0,19.0)" -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -latest -format json | ConvertFrom-Json
}
if ($vs2022) {
    Ecrire-Resultat "OK" "Visual Studio 2022 + outils C++" ("{0} {1}" -f $vs2022.displayName, $vs2022.catalog.productDisplayVersion)
    # Version de MSVC (cl.exe), C++20 complet à partir de 19.29
    $versionOutils = Get-Content (Join-Path $vs2022.installationPath "VC\Auxiliary\Build\Microsoft.VCToolsVersion.default.txt") -ErrorAction SilentlyContinue
    $cl = Join-Path $vs2022.installationPath ("VC\Tools\MSVC\{0}\bin\Hostx64\x64\cl.exe" -f "$versionOutils".Trim())
    if (Test-Path $cl) {
        $banniere = (& $cl 2>&1 | Select-Object -First 1 | Out-String)
        if ($banniere -match "(\d+)\.(\d+)\.(\d+)") {
            $msvc = [version]("{0}.{1}" -f $Matches[1], $Matches[2])
            if ($msvc -ge [version]"19.29") {
                Ecrire-Resultat "OK" "Compilateur MSVC (cl.exe)" ("{0}.{1}.{2} : C++20 pris en charge" -f $Matches[1], $Matches[2], $Matches[3])
            } else {
                Ecrire-Resultat "KO" "Compilateur MSVC (cl.exe)" ("{0} : 19.29 minimum pour C++20" -f $msvc)
            }
        }
    } else {
        Ecrire-Resultat "KO" "Compilateur MSVC (cl.exe)" "introuvable : réparer l'installation de Visual Studio"
    }
} elseif ($vs2026) {
    Ecrire-Resultat "KO" "Visual Studio 2022 + outils C++" "seul Visual Studio 2026 est installé : installer AUSSI les Build Tools 2022"
} else {
    Ecrire-Resultat "KO" "Visual Studio 2022 + outils C++" "absent : voir la note d'installation, étape 1"
}

# 3. CMake 3.25 ou plus, accessible dans le PATH, avec le générateur Visual Studio 2022
$cmake = Get-Command cmake -ErrorAction SilentlyContinue
if ($cmake) {
    $ligne = (& cmake --version | Select-Object -First 1)
    if ($ligne -match "(\d+\.\d+\.\d+)") {
        $versionCmake = [version]$Matches[1]
        if ($versionCmake -ge [version]"3.25") {
            Ecrire-Resultat "OK" "CMake" ("{0} ({1})" -f $versionCmake, $cmake.Source)
        } else {
            Ecrire-Resultat "KO" "CMake" ("{0} : 3.25 minimum (presets de workflow)" -f $versionCmake)
        }
    }
    if ((& cmake --help | Out-String) -notmatch "Visual Studio 17 2022") {
        Ecrire-Resultat "KO" "Générateur Visual Studio 17 2022" "absent de cette version de CMake"
    }
} else {
    Ecrire-Resultat "KO" "CMake" "introuvable dans le PATH : voir la note d'installation, étape 2"
}

# 4. Git (FetchContent télécharge GoogleTest, cpp-httplib et nlohmann/json avec Git)
$git = Get-Command git -ErrorAction SilentlyContinue
if ($git) {
    Ecrire-Resultat "OK" "Git" ((& git --version) -replace "git version ", "")
    $reponse = & git ls-remote https://github.com/google/googletest.git refs/tags/v1.15.2 2>&1 | Out-String
    if ($reponse -match "refs/tags/v1.15.2") {
        Ecrire-Resultat "OK" "Accès à github.com" "dépendances téléchargeables (FetchContent)"
    } else {
        Ecrire-Resultat "KO" "Accès à github.com" "impossible : proxy ou pare-feu ? (voir la note, dépannage)"
    }
} else {
    Ecrire-Resultat "KO" "Git" "introuvable dans le PATH : voir la note d'installation, étape 3"
}

# 5. Compilation et exécution d'un vrai programme C++20 (concepts, ranges, format, <=>)
if ($script:echecs -eq 0) {
    $projet = Join-Path $PSScriptRoot "verification-cpp20"
    Push-Location $projet
    $sortie = & cmake --workflow --preset windows-msvc --fresh 2>&1 | Out-String
    $code = $LASTEXITCODE
    Pop-Location
    if ($code -eq 0 -and $sortie -match "Environnement C\+\+20 conforme") {
        if ($sortie -match "(C\+\+20 OK[^\r\n]*)") { $detail = $Matches[1] } else { $detail = "programme exécuté" }
        Ecrire-Resultat "OK" "Compilation C++20 (MSVC, /W4 /WX)" $detail
    } else {
        Ecrire-Resultat "KO" "Compilation C++20 (MSVC, /W4 /WX)" "échec : voir le détail ci-dessous"
        Write-Host $sortie
    }
} else {
    Ecrire-Resultat "!!" "Compilation C++20" "non tentée : corriger d'abord les points KO"
}

# 6. Facultatif pour la partie Windows, nécessaire pour la partie Linux de la formation
$docker = Get-Command docker -ErrorAction SilentlyContinue
if ($docker) {
    $serveur = & docker version --format "{{.Server.Version}}" 2>$null
    if ($LASTEXITCODE -eq 0 -and $serveur) {
        Ecrire-Resultat "OK" "Docker Desktop (partie Linux)" ("moteur {0}" -f $serveur)
    } else {
        Ecrire-Resultat "!!" "Docker Desktop (partie Linux)" "installé mais non démarré"
    }
} else {
    Ecrire-Resultat "!!" "Docker Desktop (partie Linux)" "absent : nécessaire à partir des TP Linux"
}

Write-Host ""
if ($script:echecs -eq 0) {
    Write-Host ("Poste prêt pour la formation ({0} alerte(s))." -f $script:alertes) -ForegroundColor Green
    exit 0
}
Write-Host ("{0} point(s) bloquant(s) : voir la note d'installation." -f $script:echecs) -ForegroundColor Red
exit 1
