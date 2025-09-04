import { render, screen } from "@testing-library/react";
import DownloadPage from "../src/app/download/page"; // ← ajuste le chemin si besoin

describe("Download page", () => {
  test("le H1 'Télécharger l’app' est présent", () => {
    render(<DownloadPage />);
    const h1 = screen.getByRole("heading", { level: 1, name: /télécharger l’app/i });
    expect(h1).toBeInTheDocument();
  });

  test("les 3 cartes systèmes sont présentes (Windows, macOS, Linux)", () => {
    render(<DownloadPage />);
    expect(screen.getByRole("heading", { level: 3, name: /windows/i })).toBeInTheDocument();
    expect(screen.getByRole("heading", { level: 3, name: /macos/i })).toBeInTheDocument();
    expect(screen.getByRole("heading", { level: 3, name: /linux/i })).toBeInTheDocument();
  });

  // ===== Téléchargements : gardés en commentaires tant que les fichiers ne sont pas prêts =====
   test("macOS: le lien de téléchargement pointe vers un .zip", () => {
     render(<DownloadPage />);
     const card = screen.getByRole("heading", { level: 3, name: /macos/i }).closest("div")!;
     const link = card.querySelector('a[href]');
     expect(link!.getAttribute("href")).toMatch(/\.zip$/i);
   });

   test("Linux: le lien de téléchargement pointe vers un .AppImage", () => {
     render(<DownloadPage />);
     const card = screen.getByRole("heading", { level: 3, name: /linux/i }).closest("div")!;
     const link = card.querySelector('a[href]');
     expect(link!.getAttribute("href")).toMatch(/\.AppImage$/i);
   });

  test("lien 'Retour à l’accueil' présent", () => {
    render(<DownloadPage />);
    const retour = screen.getByRole("link", { name: /retour à l’accueil/i });
    expect(retour).toHaveAttribute("href", "/");
  });
});
