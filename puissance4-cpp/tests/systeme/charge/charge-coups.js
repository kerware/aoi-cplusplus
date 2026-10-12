// Dernière génération/modification faite par l'IA Claude le 09/10/2026 17:04:57
// ---------------------------------------------------------------------------
// Test de charge k6 — exigence EX-13 :
//   « un coup est traité en moins de 50 ms pour 95 % des requêtes, avec 50 parties jouées simultanément »
// Chaque utilisateur virtuel (VU) crée une partie puis la joue jusqu'à la victoire (7 coups), en boucle.
//
//   Windows (serveur Release lancé sur le poste) :
//     docker run --rm -i -e P4_URL=http://host.docker.internal:8080 grafana/k6 run - < charge/charge-coups.js
//   Docker Compose (serveur dans un conteneur) :
//     docker compose run --rm charge
// ---------------------------------------------------------------------------
import http from "k6/http";
import { check } from "k6";

const BASE = __ENV.P4_URL || "http://serveur:8080";
const JSON_ = { "Content-Type": "application/json" };
const VICTOIRE_VERTICALE = [0, 1, 0, 1, 0, 1, 0];

export const options = {
  scenarios: {
    parties_simultanees: {
      executor: "ramping-vus",           // montée progressive, palier, descente
      startVUs: 0,
      stages: [
        { duration: "10s", target: 50 }, // montée en charge
        { duration: "30s", target: 50 }, // palier : 50 parties jouées en même temps (EX-13)
        { duration: "5s", target: 0 },   // descente
      ],
    },
  },
  thresholds: {
    // Critères d'acceptation : le test ÉCHOUE (code de sortie 99) s'ils ne sont pas tenus
    "http_req_duration{requete:coup}": ["p(95)<50"],
    http_req_failed: ["rate<0.01"],
    checks: ["rate>0.99"],
  },
};

export default function () {
  const creation = http.post(
    `${BASE}/api/parties`,
    JSON.stringify({ joueur1: `Alice-${__VU}`, joueur2: `Bob-${__VU}` }),
    { headers: JSON_, tags: { requete: "creation" } },
  );
  check(creation, { "partie créée (201)": (r) => r.status === 201 });
  const id = creation.json("id");

  for (const colonne of VICTOIRE_VERTICALE) {
    const coup = http.post(`${BASE}/api/parties/${id}/coups`, JSON.stringify({ colonne }), {
      headers: JSON_,
      tags: { requete: "coup" },
    });
    check(coup, { "coup accepté (200)": (r) => r.status === 200 });
  }
}
