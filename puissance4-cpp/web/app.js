// Dernière génération/modification faite par l'IA Claude le 09/10/2026 13:40:00
// IHM HTML5 de l'itération 1 : crée une partie via l'API REST et affiche la grille.
"use strict";

const formulaire = document.getElementById("formulaire-partie");
const message = document.getElementById("message");

function afficherMessage(texte, estErreur) {
  message.textContent = texte;
  message.className = estErreur ? "erreur" : "";
}

function afficherPartie(partie) {
  document.getElementById("id-partie").textContent = partie.id;
  document.getElementById("joueur-courant").textContent = partie.joueurCourant;
  const grille = document.getElementById("grille");
  grille.innerHTML = "";
  for (const ligne of partie.grille) {
    for (const symbole of ligne) {
      const caseGrille = document.createElement("div");
      caseGrille.className = "case " + (symbole === "." ? "" : symbole);
      grille.appendChild(caseGrille);
    }
  }
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
