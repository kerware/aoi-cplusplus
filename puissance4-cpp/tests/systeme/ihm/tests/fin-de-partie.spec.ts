// Dernière génération/modification faite par l'IA Claude le 13/10/2026 20:23:27
import { expect, test } from "@playwright/test";
import { PagePuissance4 } from "../pages/PagePuissance4";

// 42 coups sans aucun alignement (même séquence que le TP 3)
const SEQUENCE_NULLE = [5, 4, 5, 0, 6, 2, 4, 5, 5, 0, 4, 1, 1, 0, 4, 5, 6, 5, 3, 1, 1, 2, 2, 6, 2, 6, 6, 3, 6, 2, 0, 3, 0, 3, 3, 4, 3, 1, 4, 2, 1, 0];

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

  // EX-18 : après un match nul, l'IHM n'annonce aucun vainqueur
  test("un match nul est annoncé comme tel (EX-16, EX-18)", async ({ page }) => {
    test.setTimeout(60_000);  // 42 coups joués dans l'IHM
    const p4 = new PagePuissance4(page);
    await p4.ouvrir();
    await p4.creerPartie("Alice", "Bob");

    await p4.jouerSequence(SEQUENCE_NULLE);

    await expect(p4.message).toHaveText("Match nul : la grille est pleine.");
    await expect(p4.finPartie).toHaveText("Partie terminée — match nul");
  });
});
