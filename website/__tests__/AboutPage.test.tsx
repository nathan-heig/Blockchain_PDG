import { render, screen } from "@testing-library/react";
import AboutPage from "../src/app/about/page"; // ← ajuste le chemin si besoin

describe("About page", () => {
  test("le H1 'À propos de SKBC' est présent", () => {
    render(<AboutPage />);
    const h1 = screen.getByRole("heading", { level: 1, name: /à propos de skbc/i });
    expect(h1).toBeInTheDocument();
  });

  test("les sections explicatives existent", () => {
    render(<AboutPage />);
    expect(screen.getByText(/qu’est-ce qu’une blockchain/i)).toBeInTheDocument();
    expect(screen.getByText(/notre mission/i)).toBeInTheDocument();
  });

  test("CTA bas de page: retour et téléchargement", () => {
    render(<AboutPage />);
    const retour = screen.getByRole("link", { name: /retour à l’accueil/i });
    expect(retour).toHaveAttribute("href", "/");

    const telecharger = screen.getByRole("link", { name: /télécharger l’app/i });
    expect(telecharger).toHaveAttribute("href", "/download");
  });
});
