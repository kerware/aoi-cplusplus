// Dernière génération/modification faite par l'IA Claude le 09/10/2026 16:59:44
// Tests d'IHM web (Playwright). Le serveur doit être démarré ; son adresse est lue dans P4_URL.
//   Windows : npx playwright test                 (Microsoft Edge installé, aucun téléchargement)
//   Docker  : docker compose run --rm tests-ihm   (Chromium et Firefox de l'image Playwright)
import { defineConfig, devices } from "@playwright/test";

const surWindows = process.platform === "win32";

export default defineConfig({
  testDir: "./ihm/tests",
  fullyParallel: true,             // chaque test crée sa propre partie : exécution parallèle sûre
  retries: process.env.CI ? 1 : 0, // une relance en CI, jamais en local (masquerait l'instabilité)
  reporter: [
    ["list"],
    ["html", { outputFolder: "resultats/playwright-rapport", open: "never" }],
    ["junit", { outputFile: "resultats/playwright-junit.xml" }],
  ],
  use: {
    baseURL: process.env.P4_URL ?? "http://127.0.0.1:8080",
    trace: "retain-on-failure",      // trace complète (DOM, réseau, captures) en cas d'échec
    screenshot: "only-on-failure",
  },
  projects: surWindows
    ? [{ name: "edge", use: { ...devices["Desktop Edge"], channel: "msedge" } }]
    : [
        { name: "chromium", use: { ...devices["Desktop Chrome"] } },
        { name: "firefox", use: { ...devices["Desktop Firefox"] } },
      ],
});
