// Dernière génération/modification faite par l'IA Claude le 13/10/2026 19:31:30
// IHM HTML5 de l'itération 5 : crée une partie, joue (EX-12), annonce la fin (EX-15), l'état final (EX-18)
// et affiche la version du serveur (EX-21).
"use strict";

const formulaire = document.getElementById("formulaire-partie");
const message = document.getElementById("message");
let partieCourante = null;

function afficherMessage(texte, estErreur) {
  message.textContent = texte;
  message.className = estErreur ? "erreur" : "";
}

function couleurDuJoueur(partie, nom) {
  return partie.joueur1.nom === nom ? partie.joueur1.couleur : partie.joueur2.couleur;
}

async function jouer(colonne) {
  try {
    const reponse = await fetch("/api/parties/" + partieCourante.id + "/coups", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ colonne: colonne }),
    });
    const corps = await reponse.json();
    if (!reponse.ok) {
      afficherMessage(corps.erreur, true);
      return;
    }
    const coup = corps.dernierCoup;
    if (corps.statut === "GAGNEE") {
      afficherMessage("Victoire de " + corps.vainqueur + " !", false);
    } else if (corps.statut === "NULLE") {
      afficherMessage("Match nul : la grille est pleine.", false);
    } else {
      afficherMessage(coup.joueur + " a joué en colonne " + (coup.colonne + 1), false);
    }
    afficherPartie(corps);
  } catch (erreur) {
    afficherMessage("Serveur injoignable : " + erreur.message, true);
  }
}

function afficherColonnes(partie) {
  const colonnes = document.getElementById("colonnes");
  colonnes.innerHTML = "";
  for (let c = 0; c < 7; c++) {
    const bouton = document.createElement("button");
    bouton.type = "button";
    bouton.textContent = "▼";
    bouton.title = "Jouer en colonne " + (c + 1);
    bouton.dataset.testid = "colonne-" + c;
    bouton.disabled = partie.statut !== "EN_COURS" || partie.grille[0][c] !== ".";
    bouton.addEventListener("click", () => jouer(c));
    colonnes.appendChild(bouton);
  }
}

function afficherPartie(partie) {
  partieCourante = partie;
  // EX-18 : à la fin de la partie, l'état final remplace « au tour de … »
  const enCours = partie.statut === "EN_COURS";
  document.getElementById("tour").hidden = !enCours;
  const finPartie = document.getElementById("fin-partie");
  finPartie.hidden = enCours;
  if (!enCours) {
    // EX-18 : le match nul n'a pas de vainqueur
    finPartie.textContent = partie.statut === "NULLE"
      ? "Partie terminée — match nul"
      : "Partie terminée — victoire de " + partie.vainqueur;
  }
  document.getElementById("id-partie").textContent = partie.id;
  document.getElementById("joueur-courant").textContent = partie.joueurCourant;
  document.getElementById("nombre-coups").textContent = partie.nombreDeCoups;
  document.getElementById("pastille-courant").className =
    "pastille " + (couleurDuJoueur(partie, partie.joueurCourant) === "ROUGE" ? "rouge" : "jaune");
  afficherColonnes(partie);
  const grille = document.getElementById("grille");
  grille.innerHTML = "";
  partie.grille.forEach((ligne, l) => {
    [...ligne].forEach((symbole, c) => {
      const caseGrille = document.createElement("div");
      caseGrille.className = "case " + (symbole === "." ? "" : symbole);
      caseGrille.dataset.testid = "case-" + l + "-" + c;
      grille.appendChild(caseGrille);
    });
  });
  document.getElementById("partie").hidden = false;
}

formulaire.addEventListener("submit", async (evenement) => {
  evenement.preventDefault();
  const demande = {
    joueur1: document.getElementById("joueur1").value,
    joueur2: document.getElementById("joueur2").value,
  };
  try {
    const reponse = await fetch("/api/parties", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(demande),
    });
    const corps = await reponse.json();
    if (!reponse.ok) {
      afficherMessage(corps.erreur, true);
      return;
    }
    afficherMessage("Partie créée : " + corps.joueur1.nom + " (rouge) contre " + corps.joueur2.nom + " (jaune)", false);
    afficherPartie(corps);
  } catch (erreur) {
    afficherMessage("Serveur injoignable : " + erreur.message, true);
  }
});

// EX-21 : version, commit et build du serveur, affichés en pied de page
fetch("/api/sante")
  .then((reponse) => reponse.json())
  .then((sante) => {
    document.getElementById("version").textContent =
      "· v" + sante.version + " (commit " + sante.commit.substring(0, 7) + ", build " + sante.build + ")";
  })
  .catch(() => {});
