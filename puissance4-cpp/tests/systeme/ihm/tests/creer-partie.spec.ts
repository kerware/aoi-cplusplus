// Dernière génération/modification faite par l'IA Claude le 13/10/2026 19:31:47
import { expect, test } from "@playwright/test";
import { PagePuissance4 } from "../pages/PagePuissance4";

test.describe("Créer une partie (EX-01, EX-02)", () => {
  test("une nouvelle partie affiche une grille vide et Alice commence", async ({ page }) => {
    const p4 = new PagePuissance4(page);
    await p4.ouvrir();

    await p4.creerPartie("Alice", "Bob");

    await expect(p4.idPartie).toHaveText(/^P\d{4,}$/);
    await expect(p4.joueurCourant).toHaveText("Alice");
    await expect(page.locator(".case")).toHaveCount(42);
    await expect(page.locator(".case.R, .case.J")).toHaveCount(0);
  });

  test("un nom composé d'espaces est refusé avec le message du serveur", async ({ page }) => {
    const p4 = new PagePuissance4(page);
    await p4.ouvrir();

    await p4.creerPartie("   ", "Bob");

    await expect(p4.message).toHaveText("Le nom du joueur est obligatoire");
    await expect(p4.message).toHaveClass(/erreur/);
  });

  test("le pied de page affiche la version du serveur (EX-21)", async ({ page }) => {
    const p4 = new PagePuissance4(page);
    await p4.ouvrir();

    await expect(page.getByTestId("version")).toHaveText(/^· v\d+\.\d+\.\d+ \(commit \S+, build \d+\)$/);
  });
});
