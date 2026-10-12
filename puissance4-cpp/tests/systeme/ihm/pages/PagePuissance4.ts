// Dernière génération/modification faite par l'IA Claude le 09/10/2026 16:59:44
// Page Object : toute la connaissance de la page (sélecteurs, actions) est ici.
// Les tests décrivent le comportement ; si l'IHM change, seule cette classe change.
import { expect, type Locator, type Page } from "@playwright/test";

export class PagePuissance4 {
  readonly message: Locator;
  readonly idPartie: Locator;
  readonly joueurCourant: Locator;
  readonly finPartie: Locator;
  readonly nombreCoups: Locator;

  constructor(private readonly page: Page) {
    this.message = page.getByTestId("message");
    this.idPartie = page.getByTestId("id-partie");
    this.joueurCourant = page.getByTestId("joueur-courant");
    this.finPartie = page.getByTestId("fin-partie");
    this.nombreCoups = page.getByTestId("nombre-coups");
  }

  async ouvrir() {
    await this.page.goto("/");
  }

  async creerPartie(joueur1: string, joueur2: string) {
    await this.page.getByTestId("joueur1").fill(joueur1);
    await this.page.getByTestId("joueur2").fill(joueur2);
    await this.page.getByTestId("nouvelle-partie").click();
  }

  boutonColonne(colonne: number): Locator {
    return this.page.getByTestId(`colonne-${colonne}`);
  }

  caseGrille(ligne: number, colonne: number): Locator {
    return this.page.getByTestId(`case-${ligne}-${colonne}`);
  }

  // Joue un coup et attend que l'IHM ait pris en compte la réponse du serveur
  // (attente explicite sur un état observable : pas de « sleep »).
  async jouer(colonne: number) {
    const avant = Number(await this.nombreCoups.textContent());
    await this.boutonColonne(colonne).click();
    await expect(this.nombreCoups).toHaveText(String(avant + 1));
  }

  async jouerSequence(colonnes: number[]) {
    for (const colonne of colonnes) {
      await this.jouer(colonne);
    }
  }
}
