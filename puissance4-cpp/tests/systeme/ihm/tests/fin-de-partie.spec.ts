// Dernière génération/modification faite par l'IA Claude le 09/10/2026 16:59:44
import { expect, test } from "@playwright/test";
import { PagePuissance4 } from "../pages/PagePuissance4";

test.describe("Fin de partie (EX-15, EX-17, EX-18)", () => {
  test("une victoire est annoncée et plus aucune colonne n'est jouable", async ({ page }) => {
    const p4 = new PagePuissance4(page);
    await p4.ouvrir();
    await p4.creerPartie("Alice", "Bob");

    await p4.jouerSequence([0, 1, 0, 1, 0, 1, 0]);

    await expect(p4.message).toHaveText("Victoire de Alice !");
    await expect(p4.finPartie).toHaveText("Partie terminée — victoire de Alice");
    for (let colonne = 0; colonne < 7; colonne++) {
      await expect(p4.boutonColonne(colonne)).toBeDisabled();
    }
  });
});
