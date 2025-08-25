import { render, screen } from "@testing-library/react";
import LandingPage from "../src/app/page"; // ← ajuste le chemin si pas de dossier src/

describe("Landing page", () => {
  test("smoke: le H1 'SKBC BLOCKCHAIN' est présent", () => {
    render(<LandingPage />);
    const h1 = screen.getByRole("heading", { level: 1, name: /SKBC BLOCKCHAIN/i });
    expect(h1).toBeInTheDocument();
  });

  test("la section Stats est visible et le footer est présent", () => {
    render(<LandingPage />);
    expect(screen.getByText(/stats/i)).toBeInTheDocument();
    expect(screen.getByRole("contentinfo")).toBeInTheDocument(); // footer
  });

  test("les boutons de CTA pointent vers /download et /about", () => {
    render(<LandingPage />);
    const dl = screen.getByRole("link", { name: /télécharger l’app/i });
    expect(dl).toHaveAttribute("href", "/download");

    const about = screen.getByRole("link", { name: /en savoir plus/i });
    expect(about).toHaveAttribute("href", "/about");
  });
});

// En cas de futur échec, on peut facilement activer un debug:
// afterEach(() => screen.debug());
