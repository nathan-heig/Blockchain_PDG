import { render, screen } from "@testing-library/react";
import LandingPage from "../src/app/page";

// 1. Smoke test
test("smoke: la page se rend et affiche le titre principal", () => {
  render(<LandingPage />);
  // Utilise role pour robustesse
  const heading = screen.getByRole('heading', { level: 1, name: /skbc blockchain/i });
  expect(heading).toBeInTheDocument();
});

// 2. Vérifie que les sections principales existent
test("la page contient HERO, Stats et Footer", () => {
  render(<LandingPage />);
  // HERO
  expect(screen.getByRole('heading', { level: 1, name: /skbc blockchain/i })).toBeInTheDocument();
  // Stats (case-insensitive)
  expect(screen.getByText(/stats/i)).toBeInTheDocument();
  // Footer
  expect(screen.getByRole('contentinfo')).toBeInTheDocument();
});

// 3. Vérifie le lien de téléchargement
test("le lien de téléchargement existe et pointe vers le bon ZIP", () => {
  render(<LandingPage />);
  const downloadLink = screen.getByRole('link', { name: /télécharger l’app/i });
  expect(downloadLink).toBeInTheDocument();
  expect(downloadLink).toHaveAttribute('href', expect.stringMatching(/\.zip$/));
});

// En cas de futur échec, on peut facilement activer un debug:
// afterEach(() => screen.debug());
