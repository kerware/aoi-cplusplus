// Dernière génération/modification faite par l'IA Claude le 09/10/2026 16:59:44
import { expect, test } from "@playwright/test";
import { PagePuissance4 } from "../pages/PagePuissance4";

test.describe("Jouer depuis l'IHM (EX-06, EX-07, EX-12)", () => {
  let p4: PagePuissance4;

  test.beforeEach(async ({ page }) => {
    p4 = new PagePuissance4(page);
    await p4.ouvrir();
    await p4.creerPartie("Alice", "Bob");
  });

  test("le jeton tombe en bas de la colonne et la main passe à Bob", async () => {
    await p4.jouer(3);

    await expect(p4.caseGrille(5, 3)).toHaveClass(/\bR\b/);
    await expect(p4.joueurCourant).toHaveText("Bob");
    await expect(p4.message).toHaveText("Alice a joué en colonne 4");
  });

  test("une colonne pleine ne peut plus être choisie", async () => {
    await p4.jouerSequence([0, 0, 0, 0, 0, 0]);

    await expect(p4.boutonColonne(0)).toBeDisabled();
    await expect(p4.boutonColonne(1)).toBeEnabled();
  });
});
