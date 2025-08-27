import React from "react";
import { render, screen, within } from "@testing-library/react";
import ClickerPage from "../src/app/clicker/page";

/**
 * Tests PUREMENT “mise en page” :
 * - aucun fake timer
 * - aucune interaction
 * - on vérifie la présence des sections et de quelques éléments clés
 */

describe("ClickerPage (layout)", () => {
  test("le header affiche le solde, 'par seconde :' et 'Total :'", () => {
    render(<ClickerPage />);

    // Label SKBC
    expect(screen.getAllByText(/^SKBC$/).length).toBeGreaterThanOrEqual(1);

    // Texte "par seconde :"
    expect(screen.getAllByText(/par seconde\s*:/i).length).toBeGreaterThanOrEqual(1);

    // Texte "Total :"
    expect(screen.getAllByText(/^Total\s*:/i).length).toBeGreaterThanOrEqual(1);
  });

  test("le lien 'Retour à l’accueil' et le bouton 'Réinitialiser' sont présents", () => {
    render(<ClickerPage />);

    expect(
      screen.getAllByRole("link", { name: /retour à l’accueil/i }).length
    ).toBeGreaterThanOrEqual(1);

    expect(
      screen.getAllByRole("button", { name: /réinitialiser/i }).length
    ).toBeGreaterThanOrEqual(1);
  });

  test("les sections 'Succès', 'Boutique' et 'Améliorations' sont visibles", () => {
    render(<ClickerPage />);

    expect(screen.getAllByText(/succès/i).length).toBeGreaterThanOrEqual(1);
    expect(screen.getAllByText(/boutique/i).length).toBeGreaterThanOrEqual(1);
    expect(screen.getAllByText(/améliorations/i).length).toBeGreaterThanOrEqual(1);
  });

  test("la grosse pièce cliquable est présente (bouton + image)", () => {
    render(<ClickerPage />);

    const coinBtns = screen.getAllByRole("button", {
      name: /cliquer pour miner des skbc/i,
    });
    expect(coinBtns.length).toBeGreaterThanOrEqual(1);

    const foundImg = coinBtns.some((btn) => {
      try {
        return !!within(btn).getByRole("img", { name: /pièce skbc/i });
      } catch {
        return false;
      }
    });
    expect(foundImg).toBe(true);
  });

  test("au moins un bâtiment est listé dans la 'Boutique' (Curseur)", () => {
    render(<ClickerPage />);

    const cursorBuyBtns = screen.getAllByRole("button", { name: /acheter curseur/i });
    expect(cursorBuyBtns.length).toBeGreaterThanOrEqual(1);

    const anyHasLabel = cursorBuyBtns.some((btn) => {
      return within(btn).queryAllByText(/^Curseur$/).length > 0;
    });
    expect(anyHasLabel).toBe(true);
  });

  test("au moins une amélioration est affichée (Doigt énergisé)", () => {
    render(<ClickerPage />);

    expect(
      screen.getAllByRole("button", { name: "Doigt énergisé" }).length
    ).toBeGreaterThanOrEqual(1);
  });
});
