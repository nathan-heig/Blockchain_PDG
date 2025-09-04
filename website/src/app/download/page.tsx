"use client";

import { useEffect, useMemo, useState } from "react";
import Link from "next/link";
import Image from "next/image";

// Détection simple de l'OS pour afficher "Recommandé"
function useOs() {
  const [os, setOs] = useState<"windows" | "mac" | "linux" | "other">("other");
  useEffect(() => {
    const ua = typeof navigator !== "undefined" ? navigator.userAgent.toLowerCase() : "";
    if (/windows/.test(ua)) setOs("windows");
    else if (/mac os|macintosh/.test(ua)) setOs("mac");
    else if (/linux|x11/.test(ua)) setOs("linux");
  }, []);
  return os;
}

export default function DownloadPage() {
  const os = useOs();

  // 👉 Place des logos dans /public/icons : windows.svg, mac.svg, linux.svg (ou .png)
  const builds = useMemo(
    () => [
      {
        id: "mac" as const,
        label: "macOS",
        desc: "Image disque .dmg signée (Apple Silicon & Intel).",
        href: "https://github.com/nathan-heig/Blockchain_PDG/releases/download/1.0/Blockchain_PDG_macOS.zip",
        logoSrc: "/icons/mac.png",
        badge: "DMG",
        disabled: false,
      },
      {
        id: "linux" as const,
        label: "Linux",
        desc: "AppImage x86_64 (rendu exécutable puis lancer).",
        href: "https://github.com/nathan-heig/Blockchain_PDG/releases/download/1.0/Blockchain_PDG_linux.AppImage",
        logoSrc: "/icons/linux.png",
        badge: "AppImage",
        disabled: false,
      },
      {
        id: "windows" as const,
        label: "Windows",
        desc: "Version Windows en cours de préparation.",
        // pas de href car désactivée
        logoSrc: "/icons/windows.png",
        badge: "EXE",
        disabled: true,
      },
    ],
    []
  );

  return (
    <div className="min-h-screen bg-gradient-to-b from-slate-950 via-slate-900 to-slate-950 text-slate-100">
      {/* HERO */}
      <section className="relative overflow-hidden pt-20 pb-10">
        <h1 className="text-center text-4xl sm:text-5xl md:text-6xl font-extrabold tracking-[0.08em] text-white/90">
          Télécharger l’app
        </h1>
        <p className="mt-6 text-center text-lg text-slate-300 max-w-3xl mx-auto">
          Choisissez votre système. La version correspondante à votre appareil est mise en avant.
        </p>
      </section>

      {/* CARTES DE TÉLÉCHARGEMENT */}
      <section className="max-w-5xl mx-auto px-6 pb-12 grid gap-6 sm:grid-cols-3">
        {builds.map((b) => {
          const recommended =
            (os === "windows" && b.id === "windows") ||
            (os === "mac" && b.id === "mac") ||
            (os === "linux" && b.id === "linux");

          return (
            <div
              key={b.id}
              className={`relative rounded-2xl border border-white/10 bg-white/5 p-6 overflow-hidden ${
                recommended && !b.disabled ? "ring-1 ring-emerald-400/60" : ""
              } ${b.disabled ? "opacity-70" : ""}`}
              aria-disabled={b.disabled || undefined}
            >
              {/* Ruban "Coming soon" pour Windows désactivé */}
              {b.disabled && (
                <div className="absolute top-0 left-0 right-0 bg-rose-600/90 text-white text-xs font-semibold text-center py-1">
                  Coming soon
                </div>
              )}

              {/* Badge recommandé (pas si désactivé) */}
              {recommended && !b.disabled && (
                <span className="absolute top-3 right-3 text-[11px] px-2 py-1 rounded-full bg-emerald-500/20 text-emerald-300 border border-emerald-400/30">
                  Recommandé
                </span>
              )}

              {/* Titre */}
              <h3 className="text-lg font-semibold text-center">{b.label}</h3>
              <div className="text-xs text-slate-400 text-center">{b.badge}</div>

              {/* Gros logo cliquable / non cliquable (mise en page conservée) */}
              {b.disabled ? (
                <div
                  className="mt-4 block rounded-2xl border border-white/10 bg-white/5 transition grid place-items-center aspect-square cursor-not-allowed"
                  role="button"
                  aria-label={`${b.label} indisponible`}
                  title="Indisponible pour le moment"
                >
                  <div className="relative w-28 h-28 sm:w-32 sm:h-32">
                    <Image src={b.logoSrc} alt={b.label} fill className="object-contain" />
                  </div>
                </div>
              ) : (
                <a
                  href={b.href}
                  className="mt-4 block rounded-2xl border border-white/10 bg-white/5 hover:bg-white/10 transition grid place-items-center aspect-square"
                  aria-label={`Télécharger ${b.label}`}
                >
                  <div className="relative w-28 h-28 sm:w-32 sm:h-32">
                    <Image
                      src={b.logoSrc}
                      alt={b.label}
                      fill
                      className="object-contain"
                      onError={(e) => {
                        (e.target as HTMLImageElement).style.display = "none";
                      }}
                    />
                  </div>
                </a>
              )}

              {/* Description */}
              <p className="text-slate-300 mt-4 text-sm leading-relaxed text-center">
                {b.desc}
              </p>
            </div>
          );
        })}
      </section>

      {/* AIDE + CTA */}
      <section className="max-w-5xl mx-auto px-6 pb-14">
        <div className="mt-8 flex items-center justify-center gap-4">
          <Link
            href="/"
            className="inline-flex items-center gap-2 rounded-xl border border-white/15 hover:bg-white/5 px-5 py-3 font-medium text-slate-200"
          >
            Retour à l’accueil
          </Link>
        </div>
      </section>

      {/* FOOTER */}
      <footer className="border-t border-white/10">
        <div className="max-w-6xl mx-auto px-6 py-8 text-sm text-slate-400 text-center">
          © {new Date().getFullYear()} SKBC Blockchain
        </div>
      </footer>
    </div>
  );
}
