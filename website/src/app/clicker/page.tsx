"use client";

import React, { useEffect, useMemo, useRef, useState } from "react";
import Link from "next/link";
import { flushSync } from "react-dom";

/* ================== CONFIG IMAGES ================== */
const COIN_IMG = "/clicker/coin.svg";
const TROPHY_SVG = "/clicker/trophy.svg";
const BONUS_X2_IMG = "/clicker/bonus_x2.svg";
const BONUS_X5_IMG = "/clicker/bonus_x5.svg";

/* ============ Format helpers ============ */
const fmt = new Intl.NumberFormat("fr-CH");
const formatShort = (n: number) => {
  if (n < 1000) return n.toString();
  const u = ["k", "M", "G", "T", "P", "E"];
  let i = -1;
  do {
    n /= 1000;
    i++;
  } while (n >= 1000 && i < u.length - 1);
  return `${n.toFixed(n < 10 ? 2 : n < 100 ? 1 : 0)} ${u[i]}`;
};
const pretty = (n: number) => (n % 1 === 0 ? n.toFixed(0) : n.toFixed(2));

/* ============ Small SVG icons ============ */
const LockIcon = ({ className = "" }: { className?: string }) => (
  <svg viewBox="0 0 24 24" className={className} aria-hidden="true">
    <path
      d="M12 2a5 5 0 00-5 5v3H6a2 2 0 00-2 2v8a2 2 0 002 2h12a2 2 0 002-2v-8a2 2 0 00-2-2h-1V7a5 5 0 00-5-5zm-3 8V7a3 3 0 016 0v3H9z"
      fill="currentColor"
    />
  </svg>
);
const CheckIcon = ({ className = "" }: { className?: string }) => (
  <svg viewBox="0 0 24 24" className={className}>
    <path
      d="M9 16.2l-3.5-3.5L4 14.2 9 19l11-11-1.5-1.5z"
      fill="currentColor"
    />
  </svg>
);
const ClickIcon = ({ className = "" }: { className?: string }) => (
  <svg viewBox="0 0 24 24" className={className}>
    <path d="M13 2l-2 2 2 2-2 2 2 2-2 2 2 2-2 2h11l-7-14z" fill="currentColor" />
  </svg>
);
const GearIcon = ({ className = "" }: { className?: string }) => (
  <svg viewBox="0 0 24 24" className={className}>
    <path
      d="M19.4 13a7.9 7.9 0 000-2l2.1-1.6-2-3.4-2.5 1a7.8 7.8 0 00-1.7-1l-.4-2.7H9.1l-.4 2.7a7.8 7.8 0 00-1.7 1l-2.5-1-2 3.4L4.6 11a7.9 7.9 0 000 2l-2.1 1.6 2 3.4 2.5-1a7.8 7.8 0 001.7 1l.4 2.7h4.6l.4-2.7a7.8 7.8 0 001.7-1l2.5 1 2-3.4L19.4 13zM12 15a3 3 0 110-6 3 3 0 010 6z"
      fill="currentColor"
    />
  </svg>
);

/* ============ Icônes (upgrades + trophée) ============ */
const DEFAULT_UPGRADE_ICON_DIR = "/clicker";

function UpgradeIconGraphic({ u, className = "", size = 32, alt }: { 
  u: Upgrade; className?: string; size?: number; alt?: string 
}) {
  const [failed, setFailed] = React.useState(false);

  const src = u.iconUrl ?? `${DEFAULT_UPGRADE_ICON_DIR}/${u.id}.png`;

  if (!failed) {
    return (
      <img
        src={src}
        alt={alt ?? u.name}
        className={className}
        width={size}
        height={size}
        draggable={false}
        loading="lazy"
        decoding="async"
        style={{ objectFit: "contain" }}
        onError={() => setFailed(true)}
      />
    );
  }

  // fallback en icône générique
  if (u.kind === "click") return <ClickIcon className={className} />;
  if (u.kind === "global") return <ClickIcon className={className} />;
  return <GearIcon className={className} />;
}

/** Trophée */
function TrophyIconGraphic({
  className = "",
  size = 32,
  alt = "Trophée",
}: {
  className?: string;
  size?: number;
  alt?: string;
}) {
  const [failed, setFailed] = React.useState(false);
  if (TROPHY_SVG && !failed) {
    return (
      <img
        src={TROPHY_SVG}
        alt={alt}
        className={className}
        width={size}
        height={size}
        draggable={false}
        loading="lazy"
        decoding="async"
        style={{ objectFit: "contain" }}
        onError={() => setFailed(true)}
      />
    );
  }
  return (
    <svg viewBox="0 0 24 24" className={className} aria-hidden="true">
      <path
        d="M18 4h2a1 1 0 011 1v2a5 5 0 01-5 5h-.28A6 6 0 0113 14.7V17h3a1 1 0 110 2H8a1 1 0 110-2h3v-2.3A6 6 0 018.28 12H8a5 5 0 01-5-5V5a1 1 0 011-1h2V3a1 1 0 011-1h10a1 1 0 011 1v1zm-1 2V5H7v1a1 1 0 01-1 1H4v1a3 3 0 003 3h1a1 1 0 01.96.73A4 4 0 0012 13a4 4 0 003.04-1.27A1 1 0 0116.96 12H18a3 3 0 003-3V7h-2a1 1 0 01-1-1z"
        fill="currentColor"
      />
    </svg>
  );
}

/** Icône spécifique pour succès */
function AchievementIconGraphic({
  iconUrl,
  className = "",
  size = 32,
  alt = "Succès",
}: {
  iconUrl?: string;
  className?: string;
  size?: number;
  alt?: string;
}) {
  const [failed, setFailed] = React.useState(false);
  if (iconUrl && !failed) {
    return (
      <img
        src={iconUrl}
        alt={alt}
        className={className}
        width={size}
        height={size}
        draggable={false}
        loading="lazy"
        decoding="async"
        style={{ objectFit: "contain" }}
        onError={() => setFailed(true)}
      />
    );
  }
  return <TrophyIconGraphic className={className} size={size} alt={alt} />;
}

/* ============ Coin (SVG défaut) ============ */
function SkbcCoin({
  className = "",
  style,
}: {
  className?: string;
  style?: React.CSSProperties;
}) {
  return (
    <svg
      viewBox="0 0 200 200"
      className={className}
      style={style}
      aria-hidden="true"
    >
      <defs>
        <radialGradient id="face" cx="50%" cy="38%" r="62%">
          <stop offset="0%" stopColor="#FFDFAE" />
          <stop offset="35%" stopColor="#FDBA4D" />
          <stop offset="100%" stopColor="#F59E0B" />
        </radialGradient>
        <linearGradient id="rim" x1="0%" y1="0%" x2="100%">
          <stop offset="0%" stopColor="#F7E3A0" />
          <stop offset="50%" stopColor="#CCA34A" />
          <stop offset="100%" stopColor="#F5D77A" />
        </linearGradient>
      </defs>
      <circle cx="100" cy="100" r="96" fill="url(#rim)" />
      <circle
        cx="100"
        cy="100"
        r="90"
        fill="url(#face)"
        stroke="#E59F16"
        strokeWidth="3"
      />
    </svg>
  );
}

/* ============ Coin renderer ============ */
function CoinVisual({
  className = "",
  style,
  alt = "Pièce SKBC",
}: {
  className?: string;
  style?: React.CSSProperties;
  alt?: string;
}) {
  if (COIN_IMG)
    return (
      <img
        src={COIN_IMG}
        alt={alt}
        className={className}
        style={style}
        draggable={false}
      />
    );
  return <SkbcCoin className={className} style={style} />;
}
function CoinInline({
  className = "w-4 h-4 inline-block align-[-2px]",
}: {
  className?: string;
}) {
  return (
    <img src={COIN_IMG} alt="SKBC" className={className} draggable={false} />
  );
}

/* ============ Types & données ============ */
type Building = {
  id: string;
  name: string;
  baseCost: number;
  baseCps: number;
  emoji: string;
  flavor: string;
  iconUrl?: string;
};
type Upgrade = {
  id: string;
  name: string;
  desc: string;
  cost: number;
  available: (s: GameState) => boolean;
  apply: (s: GameState) => GameState;
  taken?: boolean;
  unlockHint?: string;
  kind: "click" | "mult-building" | "global";
  targetId?: string;
  iconUrl?: string;
};
type Achievement = {
  id: string;
  name: string;
  desc: string;
  check: (
    s: GameState,
    ctx?: { upgrades: Upgrade[]; achs: Achievement[] }
  ) => boolean;
  unlocked?: boolean;
  iconUrl?: string;
};
type GameState = {
  coins: number;
  totalCoins: number;
  clickPower: number;
  buildings: Record<string, number>;
  multipliers: Record<string, number>;
  globalMult: number;
};

/** Icône de bâtiment : utilise iconUrl si fourni, sinon l’emoji */
function BuildingIconGraphic({
  b,
  className = "",
  size = 24,
  alt,
}: {
  b: Building;
  className?: string;
  size?: number;
  alt?: string;
}) {
  if (b.iconUrl) {
    return (
      <img
        src={b.iconUrl}
        alt={alt ?? b.name}
        className={className}
        width={size}
        height={size}
        draggable={false}
        loading="lazy"
        decoding="async"
        style={{ objectFit: "contain" }}
      />
    );
  }
  return <span className={`text-xl ${className}`}>{b.emoji}</span>;
}

/* ============ Bâtiments ============ */
/**
 * Courbe d'équilibrage :
 * - Début de jeu : ROI ~120–250s / bâtiment
 * - Milieu : ROI ~300–650s
 * - Fin : ROI ~600–900s (avant multiplicateurs)
 * Les upgrades par bâtiment et globales resserrent ces valeurs.
 */
const BUILDINGS: Building[] = [
  {
    id: "cursor",
    name: "Curseur",
    baseCost: 20,
    baseCps: 0.1, 
    emoji: "🖱️",
    flavor: "Clique pour toi, paresseusement.",
    iconUrl: "/clicker/buildings/cursor.png",
  },
  {
    id: "trader",
    name: "Trader",
    baseCost: 100,
    baseCps: 1,
    emoji: "📈",
    flavor: "Exécute des trades automatiques.",
    iconUrl: "/clicker/buildings/trader.png",
  },
  {
    id: "miner",
    name: "Mineur",
    baseCost: 1100,
    baseCps: 8,
    emoji: "⛏️",
    flavor: "Une carte graphique qui ronronne.",
    iconUrl: "/clicker/buildings/miner.png",
  },
  {
    id: "rig",
    name: "Rig GPU",
    baseCost: 12000,
    baseCps: 50,
    emoji: "🖥️",
    flavor: "Un rack de GPU survitaminés.",
    iconUrl: "/clicker/buildings/rig.png",
  },
  {
    id: "pool",
    name: "Pool de minage",
    baseCost: 45000,
    baseCps: 120,
    emoji: "🌐",
    flavor: "Regroupe la puissance de calcul.",
    iconUrl: "/clicker/buildings/pool.png",
  },
  {
    id: "dc",
    name: "Datacenter",
    baseCost: 80000,
    baseCps: 210,
    emoji: "🏢",
    flavor: "Rangées de serveurs qui soufflent.",
    iconUrl: "/clicker/buildings/dc.png",
  },
  {
    id: "farm",
    name: "Usine",
    baseCost: 130000,
    baseCps: 300,
    emoji: "🏭",
    flavor: "Un hangar entier, ça chauffe.",
    iconUrl: "/clicker/buildings/farm.png",
  },
  {
    id: "lab",
    name: "Laboratoire",
    baseCost: 500000,
    baseCps: 1000,
    emoji: "🧪",
    flavor: "R&D pour grappiller des SKBC.",
    iconUrl: "/clicker/buildings/lab.png",
  },
  {
    id: "solar",
    name: "Parc solaire",
    baseCost: 900000,
    baseCps: 1600,
    emoji: "☀️",
    flavor: "Énergie verte pour miner.",
    iconUrl: "/clicker/buildings/solar.png",
  },
  {
    id: "super",
    name: "Supercalculateur",
    baseCost: 7500000,
    baseCps: 9000,
    emoji: "🧠",
    flavor: "Des pétaflops pour des SKBC.",
    iconUrl: "/clicker/buildings/super.png",
  },
  {
    id: "quantum",
    name: "Calculateur quantique",
    baseCost: 15000000,
    baseCps: 22000,
    emoji: "⚛️",
    flavor: "Qubits au service du hash.",
    iconUrl: "/clicker/buildings/quantum.png",
  }
];

/* ============ Améliorations ============ */
const UPGRADES: Upgrade[] = [
  /* ===== CLIC ===== */
  {
    id: "clickx2",
    name: "Doigt énergisé",
    desc: "Gain par clic ×2.",
    cost: 100,
    available: (s) => s.totalCoins >= 50,
    apply: (s) => ({ ...s, clickPower: s.clickPower * 2 }),
    unlockHint: "Atteins 50 SKBC cumulés",
    kind: "click",
  },
  {
    id: "clickx3",
    name: "Gant de mineur",
    desc: "Gain par clic ×3.",
    cost: 1200,
    available: (s) => s.totalCoins >= 1500,
    apply: (s) => ({ ...s, clickPower: s.clickPower * 3 }),
    unlockHint: "Atteins 1500 SKBC cumulés",
    kind: "click",
  },
  {
    id: "clickx5",
    name: "Exo-souris",
    desc: "Gain par clic ×5.",
    cost: 20000,
    available: (s) => s.totalCoins >= 50000,
    apply: (s) => ({ ...s, clickPower: s.clickPower * 5 }),
    unlockHint: "Atteins 50k SKBC cumulés",
    kind: "click",
  },
  {
    id: "clickx10",
    name: "Doigts bioniques",
    desc: "Gain par clic ×10.",
    cost: 100000,
    available: (s) => s.totalCoins >= 250000,
    apply: (s) => ({ ...s, clickPower: s.clickPower * 10 }),
    unlockHint: "Atteins 250k SKBC cumulés",
    kind: "click",
  },

  /* ===== MULTIPLICATEURS PAR BÂTIMENT ===== */
  {
    id: "cursor+5x",
    name: "Curseurs suprèmes",
    desc: "Les Curseurs produisent 5× plus.",
    cost: 60000,
    available: (s) => (s.buildings["cursor"] ?? 0) >= 50,
    apply: (s) => ({
      ...s,
      multipliers: { ...s.multipliers, cursor: (s.multipliers.cursor ?? 1) * 5 },
    }),
    unlockHint: "Possède 50 Curseurs",
    kind: "mult-building",
    targetId: "cursor",
  },

  {
    id: "cursor+2x",
    name: "Curseurs lubrifiés",
    desc: "Les Curseurs produisent 2× plus.",
    cost: 1000,
    available: (s) => (s.buildings["cursor"] ?? 0) >= 20,
    apply: (s) => ({
      ...s,
      multipliers: { ...s.multipliers, cursor: (s.multipliers.cursor ?? 1) * 2 },
    }),
    unlockHint: "Possède 20 Curseurs",
    kind: "mult-building",
    targetId: "cursor",
  },
  {
    id: "trader+2x",
    name: "IA de trading",
    desc: "Les Traders produisent 2× plus.",
    cost: 2500,
    available: (s) => (s.buildings["trader"] ?? 0) >= 15,
    apply: (s) => ({
      ...s,
      multipliers: { ...s.multipliers, trader: (s.multipliers.trader ?? 1) * 2 },
    }),
    unlockHint: "Possède 15 Traders",
    kind: "mult-building",
    targetId: "trader",
  },
  {
    id: "miner+2x",
    name: "Pilotes optimisés",
    desc: "Les Mineurs produisent 2× plus.",
    cost: 7000,
    available: (s) => (s.buildings["miner"] ?? 0) >= 10,
    apply: (s) => ({
      ...s,
      multipliers: { ...s.multipliers, miner: (s.multipliers.miner ?? 1) * 2 },
    }),
    unlockHint: "Possède 10 Mineurs",
    kind: "mult-building",
    targetId: "miner",
  },
  {
    id: "rig+2x",
    name: "Overclock stable",
    desc: "Les Rigs GPU produisent 2× plus.",
    cost: 35000,
    available: (s) => (s.buildings["rig"] ?? 0) >= 8,
    apply: (s) => ({
      ...s,
      multipliers: { ...s.multipliers, rig: (s.multipliers.rig ?? 1) * 2 },
    }),
    unlockHint: "Possède 8 Rigs",
    kind: "mult-building",
    targetId: "rig",
  },
  {
    id: "pool+2x",
    name: "Optimisation du pool",
    desc: "Les Pools produisent 2× plus.",
    cost: 120000,
    available: (s) => (s.buildings["pool"] ?? 0) >= 5,
    apply: (s) => ({
      ...s,
      multipliers: { ...s.multipliers, pool: (s.multipliers.pool ?? 1) * 2 },
    }),
    unlockHint: "Possède 5 Pools",
    kind: "mult-building",
    targetId: "pool",
  },
  {
    id: "dc+2x",
    name: "Réseau 100 Gbps",
    desc: "Les Datacenters produisent 2× plus.",
    cost: 240000,
    available: (s) => (s.buildings["dc"] ?? 0) >= 5,
    apply: (s) => ({
      ...s,
      multipliers: { ...s.multipliers, dc: (s.multipliers.dc ?? 1) * 2 },
    }),
    unlockHint: "Possède 5 Datacenters",
    kind: "mult-building",
    targetId: "dc",
  },
  {
    id: "farm+2x",
    name: "Refroidissement liquide",
    desc: "Les Usines produisent 2× plus.",
    cost: 600000,
    available: (s) => (s.buildings["farm"] ?? 0) >= 5,
    apply: (s) => ({
      ...s,
      multipliers: { ...s.multipliers, farm: (s.multipliers.farm ?? 1) * 2 },
    }),
    unlockHint: "Possède 5 Fermes",
    kind: "mult-building",
    targetId: "farm",
  },
  {
    id: "solar+2x",
    name: "Onduleurs haut rendement",
    desc: "Les Parcs solaires produisent 2× plus.",
    cost: 1800000,
    available: (s) => (s.buildings["solar"] ?? 0) >= 3,
    apply: (s) => ({
      ...s,
      multipliers: { ...s.multipliers, solar: (s.multipliers.solar ?? 1) * 2 },
    }),
    unlockHint: "Possède 3 Parcs solaires",
    kind: "mult-building",
    targetId: "solar",
  },
  {
    id: "super+2x",
    name: "CUDA maison",
    desc: "Les Supercalculateurs produisent 2× plus.",
    cost: 9500000,
    available: (s) => (s.buildings["super"] ?? 0) >= 2,
    apply: (s) => ({
      ...s,
      multipliers: { ...s.multipliers, super: (s.multipliers.super ?? 1) * 2 },
    }),
    unlockHint: "Possède 2 Supercalculateurs",
    kind: "mult-building",
    targetId: "super",
  },
  {
    id: "lab+2x",
    name: "Subventions R&D",
    desc: "Les Laboratoires produisent 2× plus.",
    cost: 1000000,
    available: (s) => (s.buildings["lab"] ?? 0) >= 4,
    apply: (s) => ({
      ...s,
      multipliers: { ...s.multipliers, lab: (s.multipliers.lab ?? 1) * 2 },
    }),
    unlockHint: "Possède 4 Laboratoires",
    kind: "mult-building",
    targetId: "lab",
  },
  {
    id: "quantum+2x",
    name: "Supraconducteurs purs",
    desc: "Les Calculateurs quantiques produisent 2× plus.",
    cost: 30000000,
    available: (s) => (s.buildings["quantum"] ?? 0) >= 2,
    apply: (s) => ({
      ...s,
      multipliers: { ...s.multipliers, quantum: (s.multipliers.quantum ?? 1) * 2 },
    }),
    unlockHint: "Possède 2 Calculateurs quantiques",
    kind: "mult-building",
    targetId: "quantum",
  },

  /* ===== BONUS GLOBAUX ===== */
  {
    id: "ram-16",
    name: "+16 Go de RAM",
    desc: "+5% de CPS global.",
    cost: 100000,
    available: (s) => s.totalCoins >= 150000,
    apply: (s) => ({ ...s, globalMult: s.globalMult * 1.05 }),
    unlockHint: "Atteins 150k SKBC",
    kind: "global",
  },
  {
    id: "ram-32",
    name: "+32 Go de RAM",
    desc: "+10% de CPS global.",
    cost: 500000,
    available: (s) => s.totalCoins >= 1000000,
    apply: (s) => ({ ...s, globalMult: s.globalMult * 1.1 }),
    unlockHint: "Atteins 1M SKBC",
    kind: "global",
  },
];

/* ============ Succès ============ */
const ACHIEVEMENTS: Achievement[] = [
  /* Début */
  {
    id: "first-click",
    name: "Premier coup de pioche",
    desc: "Clique une fois le coin.",
    check: (s) => s.totalCoins >= 1,
    iconUrl: "/clicker/bronze_medal.svg",
  },
  {
    id: "100-total",
    name: "Ça clique bien",
    desc: "Atteins 100 SKBC cumulés.",
    check: (s) => s.totalCoins >= 100,
    iconUrl: "/clicker/bronze_medal.svg",
  },
  {
    id: "1k",
    name: "Premier millier",
    desc: "Atteins 1 000 SKBC cumulés.",
    check: (s) => s.totalCoins >= 1000,
    iconUrl: "/clicker/bronze_medal.svg",
  },
  {
    id: "first-auto",
    name: "Ça mine tout seul",
    desc: "Achète un bâtiment.",
    check: (s) => Object.values(s.buildings ?? {}).some((n) => n > 0),
    iconUrl: "/clicker/bronze_medal.svg",
  },

  /* Milieu */
  {
    id: "20-bats",
    name: "Petit empire",
    desc: "Atteins 20 bâtiments (total).",
    check: (s) => totalBuildings(s) >= 20,
    iconUrl: "/clicker/silver_medal.svg",
  },
  {
    id: "10k",
    name: "Cinq chiffres",
    desc: "Atteins 10 000 SKBC cumulés.",
    check: (s) => s.totalCoins >= 10_000,
    iconUrl: "/clicker/silver_medal.svg",
  },
  {
    id: "50-bats",
    name: "Grande ferme",
    desc: "Atteins 50 bâtiments (total).",
    check: (s) => totalBuildings(s) >= 50,
    iconUrl: "/clicker/silver_medal.svg",
  },
  {
    id: "100k",
    name: "Six chiffres",
    desc: "Atteins 100 000 SKBC cumulés.",
    check: (s) => s.totalCoins >= 100_000,
    iconUrl: "/clicker/silver_medal.svg",
  },
  {
    id: "6-upgrades",
    name: "Geek en herbe",
    desc: "Achète 6 améliorations.",
    check: (_s, ctx) =>
      !!ctx?.upgrades && ctx.upgrades.filter((u) => u.taken).length >= 6,
    iconUrl: "/clicker/silver_medal.svg",
  },

  /* Avancé */
  {
    id: "1m",
    name: "Sept chiffres",
    desc: "Atteins 1 000 000 SKBC cumulés.",
    check: (s) => s.totalCoins >= 1_000_000,
    iconUrl: "/clicker/gold_medal.svg",
  },
  {
    id: "100-bats",
    name: "Mega ferme",
    desc: "Atteins 100 bâtiments (total).",
    check: (s) => totalBuildings(s) >= 100,
    iconUrl: "/clicker/gold_medal.svg",
  },
  {
    id: "12-upgrades",
    name: "Ingé système",
    desc: "Achète 12 améliorations.",
    check: (_s, ctx) =>
      !!ctx?.upgrades && ctx.upgrades.filter((u) => u.taken).length >= 12,
    iconUrl: "/clicker/gold_medal.svg",
  },
  {
    id: "150-bats",
    name: "Hyper ferme",
    desc: "Atteins 150 bâtiments (total).",
    check: (s) => totalBuildings(s) >= 150,
    iconUrl: "/clicker/gold_medal.svg",
  },

  /* Endgame */
  {
    id: "200-bats",
    name: "Ultra ferme",
    desc: "Atteins 200 bâtiments (total).",
    check: (s) => totalBuildings(s) >= 200,
    iconUrl: "/clicker/trophy.svg",
  },
  {
    id: "50m",
    name: "Gros bonnet",
    desc: "Atteins 50 000 000 SKBC cumulés.",
    check: (s) => s.totalCoins >= 50_000_000,
    iconUrl: "/clicker/trophy.svg",
  },
  {
    id: "all-upgrades",
    name: "Full upgrade",
    desc: "Achète toutes les améliorations.",
    check: (_s, ctx) =>
      Boolean(ctx?.upgrades.length && ctx.upgrades.every((u) => !!u.taken)),
    iconUrl: "/clicker/trophy.svg",
  },
  {
    id: "all-buildings",
    name: "Collectionneur",
    desc: "Achète chaque type de bâtiment.",
    check: (s) => BUILDINGS.every((b) => (s.buildings[b.id] ?? 0) > 0),
    iconUrl: "/clicker/trophy.svg",
  },

  {
    id: "all-achievements",
    name: "Grand Maître SKBC",
    desc: "Débloque tous les autres succès.",
    check: (_s, ctx) => {
      if (!ctx?.achs?.length) return false;
      return ctx.achs
        .filter((a) => a.id !== "all-achievements")
        .every((a) => !!a.unlocked);
    },
    iconUrl: "/clicker/diams_trophy.svg",
  },
];

/* ============ Helpers ============ */
const nextCost = (base: number, owned: number) =>
  Math.ceil(base * Math.pow(1.15, owned));
const SAVE_KEY = "skbc_clicker_v2";
const LAST_TICK_KEY = "skbc_lastTick";
const LAST_CPS_KEY = "skbc_lastCps";
const BOOST_UNTIL_KEY = "skbc_boostUntil";
const BOOST_MULT_KEY = "skbc_boostMult";
const totalBuildings = (s: GameState) =>
  Object.values(s.buildings ?? {}).reduce((a, b) => a + b, 0);

function totalCps(state: GameState) {
  const sum = BUILDINGS.reduce((acc, b) => {
    const owned = state.buildings[b.id] || 0;
    const mult = state.multipliers[b.id] || 1;
    return acc + owned * b.baseCps * mult;
  }, 0);
  return sum * state.globalMult;
}
function perBuildingCps(state: GameState, b: Building) {
  const mult = state.multipliers[b.id] || 1;
  return b.baseCps * mult * state.globalMult;
}

/** Déverrouillage progressif des bâtiments */
function isBuildingUnlocked(state: GameState, idx: number) {
  if (idx === 0) return true;
  const prev = BUILDINGS[idx - 1];
  return state.totalCoins >= prev.baseCost;
}

/* === hooks décor & utilitaires === */
function useParticles() {
  const [parts, setParts] = useState<
    { id: number; x: number; y: number; text: string }[]
  >([]);
  const idRef = useRef(1);
  const spawn = (x: number, y: number, text: string) => {
    const id = idRef.current++;
    setParts((ps) => [...ps, { id, x, y, text }]);
    setTimeout(() => setParts((ps) => ps.filter((p) => p.id !== id)), 1000);
  };
  return { parts, spawn };
}
function useSize<T extends HTMLElement>(ref: React.RefObject<T | null>) {
  const [size, setSize] = useState({ width: 0, height: 0 });
  useEffect(() => {
    const el = ref.current;
    if (!el) return;
    const update = () =>
      setSize({ width: el.clientWidth, height: el.clientHeight });
    update();
    const ro = new ResizeObserver(update);
    ro.observe(el);
    return () => ro.disconnect();
  }, [ref]);
  return size;
}
function useFallingCoinsArea(width: number, height: number) {
  type FC = {
    id: number;
    x: number;
    y: number;
    size: number;
    speed: number;
    drift: number;
    opacity: number;
  };
  const [coins, setCoins] = useState<FC[]>([]);
  const idRef = useRef(1);
  useEffect(() => {
    if (!width || !height) return;
    const spawn = setInterval(() => {
      const id = idRef.current++;
      setCoins((arr) => [
        ...arr,
        {
          id,
          x: Math.random() * (width - 60) + 30,
          y: -50,
          size: Math.random() * 36 + 20,
          speed: Math.random() * 0.5 + 0.35,
          drift: (Math.random() - 0.5) * 0.6,
          opacity: Math.random() * 0.25 + 0.15,
        },
      ]);
    }, 1100);
    const anim = setInterval(() => {
      setCoins((arr) =>
        arr
          .map((c) => ({
            ...c,
            y: c.y + c.speed * 8,
            x: c.x + c.drift,
          }))
          .filter((c) => c.y < height + 60)
      );
    }, 50);
    return () => {
      clearInterval(spawn);
      clearInterval(anim);
    };
  }, [width, height]);
  return coins;
}

/* ============ Sauvegarde low-level ============ */
type SaveShape = {
  state: GameState;
  upgrades: { id: string; taken?: boolean }[];
  achs: { id: string; unlocked?: boolean }[];
};
function loadSave(): SaveShape | null {
  try {
    const raw =
      typeof window !== "undefined" ? localStorage.getItem(SAVE_KEY) : null;
    return raw ? (JSON.parse(raw) as SaveShape) : null;
  } catch {
    return null;
  }
}
function saveNow(payload: SaveShape) {
  try {
    localStorage.setItem(SAVE_KEY, JSON.stringify(payload));
  } catch {}
}

/* ============ Page ============ */
export default function ClickerPage() {
  const defaultState: GameState = {
    coins: 0,
    totalCoins: 0,
    clickPower: 1,
    buildings: {},
    multipliers: {},
    globalMult: 1,
  };

  // Lazy init
  const [state, setState] = useState<GameState>(() => {
    if (typeof window === "undefined") return defaultState;
    const saved = loadSave();
    if (saved?.state) {
      const s = saved.state;
      return {
        ...defaultState,
        ...s,
        buildings: s.buildings ?? {},
        multipliers: s.multipliers ?? {},
        globalMult: s.globalMult ?? 1,
      };
    }
    return defaultState;
  });

  const [upgrades, setUpgrades] = useState<Upgrade[]>(() => {
    if (typeof window === "undefined") return UPGRADES;
    const saved = loadSave();
    if (saved?.upgrades) {
      const taken = new Set(
        saved.upgrades.filter((u) => u.taken).map((u) => u.id)
      );
      return UPGRADES.map((u) => ({ ...u, taken: taken.has(u.id) }));
    }
    return UPGRADES;
  });
  const [achs, setAchs] = useState<Achievement[]>(() => {
    if (typeof window === "undefined") return ACHIEVEMENTS;
    const saved = loadSave();
    if (saved?.achs) {
      const unlocked = new Set(
        saved.achs.filter((a) => a.unlocked).map((a) => a.id)
      );
      return ACHIEVEMENTS.map((a) => ({
        ...a,
        unlocked: unlocked.has(a.id),
      }));
    }
    return ACHIEVEMENTS;
  });

  /* ===== Boost ===== */
  const [boost, setBoost] = useState<{
    active: boolean;
    until: number;
    mult: number;
  }>({ active: false, until: 0, mult: 1 });

  // flags
  const readyToToastRef = useRef(false);
  const interactedRef = useRef(false);
  const [hydrated, setHydrated] = useState(false);
  const [mounted, setMounted] = useState(false);

  const skipSaveRef = useRef(false);
  const mountedRef = useRef(false);

  const coinWrapRef = useRef<HTMLDivElement | null>(null);

  /* ===== LOAD ===== */
  useEffect(() => {
    const saved = loadSave();

    if (saved?.state) {
      const s = saved.state;
      setState((prev) => ({
        ...prev,
        ...s,
        buildings: s.buildings ?? {},
        multipliers: s.multipliers ?? {},
        globalMult: s.globalMult ?? 1,
      }));
    }

    const u = parseInt(localStorage.getItem(BOOST_UNTIL_KEY) || "0", 10);
    const m = parseFloat(localStorage.getItem(BOOST_MULT_KEY) || "1");
    if (u > Date.now() && m > 1) setBoost({ active: true, until: u, mult: m });

    mountedRef.current = true;
    readyToToastRef.current = true;
    setHydrated(true);
    setMounted(true);
  }, []);

  // Boost timer
  const [now, setNow] = useState(Date.now());
  useEffect(() => {
    const t = setInterval(() => setNow(Date.now()), 250);
    return () => clearInterval(t);
  }, []);
  useEffect(() => {
    if (boost.active && Date.now() > boost.until)
      setBoost({ active: false, until: 0, mult: 1 });
  }, [now, boost]);
  const boostMult = boost.active ? boost.mult : 1;

  // Bonus coins
  type BonusCoin = { x: number; y: number; kind: "x2" | "x5" };
  const [bonusCoin, setBonusCoin] = useState<BonusCoin | null>(null);

  useEffect(() => {
    const int = setInterval(() => {
      if (bonusCoin) return;
      if (Math.random() < 0.04) {
        const kind: BonusCoin["kind"] = Math.random() < 0.25 ? "x5" : "x2";

        const placeCoin = () => {
          const vw = window.innerWidth;
          const vh = window.innerHeight;
          const margin = 40;
          const x = Math.random() * (vw - margin * 2) + margin;
          const y = Math.random() * (vh - margin * 2) + margin;

          const wrap = coinWrapRef.current;
          if (wrap) {
            const r = wrap.getBoundingClientRect();
            const pad = 20;
            const inside =
              x > r.left - pad &&
              x < r.right + pad &&
              y > r.top - pad &&
              y < r.bottom + pad;
            if (inside) return false;
          }
          setBonusCoin({ x, y, kind });
          return true;
        };

        let ok = placeCoin();
        let tries = 0;
        while (!ok && tries < 8) {
          ok = placeCoin();
          tries++;
        }
      }
    }, 10000);
    return () => clearInterval(int);
  }, [bonusCoin]);

  const collectBonus = () => {
    interactedRef.current = true;
    if (!bonusCoin) return;
    const mult = bonusCoin.kind === "x5" ? 5 : 2;
    const until = Date.now() + 15_000;
    setBoost({ active: true, until, mult });
    localStorage.setItem(BOOST_UNTIL_KEY, String(until));
    localStorage.setItem(BOOST_MULT_KEY, String(mult));
    setBonusCoin(null);
  };

  // Refs pour save
  const stateRef = useRef(state);
  const upgradesRef = useRef(upgrades);
  const achsRef = useRef(achs);
  const boostRef = useRef(boost);
  useEffect(() => {
    stateRef.current = state;
  }, [state]);
  useEffect(() => {
    upgradesRef.current = upgrades;
  }, [upgrades]);
  useEffect(() => {
    achsRef.current = achs;
  }, [achs]);
  useEffect(() => {
    boostRef.current = boost;
  }, [boost]);

  // sauvegarde immédiate
  const saveImmediate = (
    stateOverride?: GameState,
    upgradesOverride?: Upgrade[],
    achsOverride?: Achievement[],
    boostOverride?: { active: boolean; until: number; mult: number }
  ) => {
    if (skipSaveRef.current) return;
    const st = stateOverride ?? stateRef.current;
    const ups = upgradesOverride ?? upgradesRef.current;
    const acs = achsOverride ?? achsRef.current;
    const bst = boostOverride ?? boostRef.current;

    const payload: SaveShape = {
      state: st,
      upgrades: ups.map((u) => ({ id: u.id, taken: !!u.taken })),
      achs: acs.map((a) => ({ id: a.id, unlocked: !!a.unlocked })),
    };
    saveNow(payload);
    localStorage.setItem(LAST_TICK_KEY, String(Date.now()));
    localStorage.setItem(
      LAST_CPS_KEY,
      String(totalCps(st) * (bst.active ? bst.mult : 1))
    );
    localStorage.setItem(BOOST_UNTIL_KEY, String(bst.until || 0));
    localStorage.setItem(BOOST_MULT_KEY, String(bst.mult || 1));
  };

  // CPS
  const cps = useMemo(
    () => totalCps(state),
    [state.buildings, state.multipliers, state.globalMult]
  );
  const effectiveCps = cps * boostMult;

  // Rattrapage off-tab
  useEffect(() => {
    const last = parseInt(localStorage.getItem(LAST_TICK_KEY) || "0", 10);
    const lastCps = parseFloat(localStorage.getItem(LAST_CPS_KEY) || "0");
    if (last > 0 && lastCps > 0) {
      const diff = (Date.now() - last) / 1000;
      if (diff > 0)
        setState((s) => ({
          ...s,
          coins: s.coins + lastCps * diff,
          totalCoins: s.totalCoins + lastCps * diff,
        }));
    }
  }, []);

  // boucle CPS
  useEffect(() => {
    const int = setInterval(() => {
      setState((s) => ({
        ...s,
        coins: s.coins + effectiveCps / 10,
        totalCoins: s.totalCoins + effectiveCps / 10,
      }));
      if (!skipSaveRef.current) {
        localStorage.setItem(LAST_TICK_KEY, String(Date.now()));
        localStorage.setItem(LAST_CPS_KEY, String(effectiveCps));
      }
    }, 100);
    return () => clearInterval(int);
  }, [effectiveCps]);

  // toasts de succès
  const [toasts, setToasts] = useState<
    { id: number; title: string; desc?: string }[]
  >([]);
  const toastId = useRef(1);
  const pushToast = (title: string, desc?: string) => {
    const id = toastId.current++;
    setToasts((t) => [...t, { id, title, desc }]);
    setTimeout(() => setToasts((t) => t.filter((x) => x.id !== id)), 2400);
  };
  useEffect(() => {
    const newly = achs.filter(
      (a) => !a.unlocked && a.check(state, { upgrades, achs })
    );
    if (!newly.length) return;

    setAchs((arr) =>
      arr.map((a) => (newly.includes(a) ? { ...a, unlocked: true } : a))
    );

    if (readyToToastRef.current && interactedRef.current) {
      newly.forEach((a) => pushToast(a.name, a.desc));
    }
  }, [state, achs, upgrades]);

  /* ===== Sauvegarde “normale” ===== */
  const buildSnapshot = (): SaveShape => {
    const cur = stateRef.current;
    const ups = upgradesRef.current;
    const acs = achsRef.current;
    return {
      state: cur,
      upgrades: ups.map((u) => ({ id: u.id, taken: !!u.taken })),
      achs: acs.map((a) => ({ id: a.id, unlocked: !!a.unlocked })),
    };
  };
  const saveSnapshot = () => {
    if (skipSaveRef.current) return;
    const bst = boostRef.current;
    const payload = buildSnapshot();
    saveNow(payload);
    localStorage.setItem(LAST_TICK_KEY, String(Date.now()));
    localStorage.setItem(
      LAST_CPS_KEY,
      String(totalCps(payload.state) * (bst.active ? bst.mult : 1))
    );
    localStorage.setItem(BOOST_UNTIL_KEY, String(bst.until || 0));
    localStorage.setItem(BOOST_MULT_KEY, String(bst.mult || 1));
  };

  // périodique
  useEffect(() => {
    if (!hydrated) return;
    const tick = setInterval(saveSnapshot, 2000);
    return () => clearInterval(tick);
  }, [hydrated]);

  // à chaque changement
  useEffect(() => {
    if (!mountedRef.current || !hydrated || skipSaveRef.current) return;
    saveSnapshot();
  }, [hydrated, state, upgrades, achs, boost]);

  // events navigation / perte focus
  useEffect(() => {
    const onPageHide = () => saveSnapshot();
    const onVisibility = () => {
      if (document.hidden) saveSnapshot();
    };
    const onBeforeUnload = () => saveSnapshot();
    const onBlur = () => saveSnapshot();
    const onPopState = () => saveSnapshot();

    window.addEventListener("pagehide", onPageHide);
    document.addEventListener("visibilitychange", onVisibility);
    window.addEventListener("beforeunload", onBeforeUnload);
    window.addEventListener("blur", onBlur);
    window.addEventListener("popstate", onPopState);

    return () => {
      window.removeEventListener("pagehide", onPageHide);
      document.removeEventListener("visibilitychange", onVisibility);
      window.removeEventListener("beforeunload", onBeforeUnload);
      window.removeEventListener("blur", onBlur);
      window.removeEventListener("popstate", onPopState);
      if (!skipSaveRef.current) saveSnapshot();
    };
  }, []);

  // Sauvegarder avant navigation interne
  useEffect(() => {
    const onAnyLinkPointerDown = (e: PointerEvent) => {
      const target = e.target as HTMLElement | null;
      const anchor = target?.closest("a");
      if (anchor && anchor.getAttribute("href")) saveImmediate();
    };
    document.addEventListener("pointerdown", onAnyLinkPointerDown, true);
    return () =>
      document.removeEventListener("pointerdown", onAnyLinkPointerDown, true);
  }, []);

  // reset
  const hardReset = () => {
    skipSaveRef.current = true;
    try {
      localStorage.removeItem(SAVE_KEY);
      localStorage.removeItem(LAST_TICK_KEY);
      localStorage.removeItem(LAST_CPS_KEY);
      localStorage.removeItem(BOOST_UNTIL_KEY);
      localStorage.removeItem(BOOST_MULT_KEY);
    } catch {}
    setState({
      coins: 0,
      totalCoins: 0,
      clickPower: 1,
      buildings: {},
      multipliers: {},
      globalMult: 1,
    });
    setUpgrades(UPGRADES.map((u) => ({ ...u, taken: false })));
    setAchs(ACHIEVEMENTS.map((a) => ({ ...a, unlocked: false })));
    setBoost({ active: false, until: 0, mult: 1 });
    if (typeof window !== "undefined") location.reload();
  };

  // Particules clic
  const { parts, spawn } = useParticles();
  const handleClick = (e: React.MouseEvent) => {
    interactedRef.current = true;
    const gain = state.clickPower * boostMult;
    setState((s) => ({
      ...s,
      coins: s.coins + gain,
      totalCoins: s.totalCoins + gain,
    }));
    const wrap = coinWrapRef.current;
    if (wrap) {
      const rect = wrap.getBoundingClientRect();
      const x = e.clientX - rect.left;
      const y = e.clientY - rect.top;
      spawn(x, y, `+${pretty(gain)}`);
    }
  };

  // Achats
  const buyBuilding = (b: Building) => {
    interactedRef.current = true;
    const s = stateRef.current;
    const owned = s.buildings[b.id] || 0;
    const cost = nextCost(b.baseCost, owned);
    if (s.coins < cost) return;

    const nextState: GameState = {
      ...s,
      coins: s.coins - cost,
      buildings: { ...s.buildings, [b.id]: owned + 1 },
    };
    flushSync(() => setState(nextState));
    saveImmediate(nextState);
  };

  const buyUpgrade = (u: Upgrade) => {
    interactedRef.current = true;
    const s = stateRef.current;
    const ups = upgradesRef.current;
    if (u.taken || !u.available(s) || s.coins < u.cost) return;

    const nextState: GameState = u.apply({ ...s, coins: s.coins - u.cost });
    const nextUpgrades: Upgrade[] = ups.map((x) =>
      x.id === u.id ? { ...x, taken: true } : x
    );

    flushSync(() => {
      setState(nextState);
      setUpgrades(nextUpgrades);
    });
    saveImmediate(nextState, nextUpgrades);
  };

  // panneau principal décor
  const panelRef = useRef<HTMLDivElement | null>(null);
  const panelSize = useSize(panelRef);
  const falling = useFallingCoinsArea(panelSize.width, panelSize.height);

  // Tri upgrades
  const sortedUpgrades = useMemo(
    () => [...upgrades].sort((a, b) => a.cost - b.cost),
    [upgrades]
  );

  // “Retour”
  const handleReturnClick: React.MouseEventHandler<HTMLAnchorElement> = () => {
    flushSync(() => {});
    saveImmediate();
  };

  return (
    <div className="min-h-screen bg-gradient-to-b from-slate-950 via-slate-900 to-slate-950 text-slate-100 relative overflow-visible">
      {/* header */}
      <div className="sticky top-0 z-40 bg-slate-950/50 backdrop-blur border-b border-white/10">
        <div className="max-w-7xl mx-auto flex items-center justify-between px-4 py-3">
          <div className="flex items-center gap-3">
            <Link
              href="/"
              onClick={handleReturnClick}
              className="inline-flex items-center gap-2 rounded-xl border border-white/15 hover:bg-white/5 px-5 py-3 font-medium text-slate-200"
            >
              Retour à l’accueil
            </Link>
            <button
              onClick={hardReset}
              className="text-xs px-2 py-1 rounded border border-white/10 text-slate-300 hover:bg:white/5 hover:bg-white/5"
              title="Supprimer la sauvegarde locale"
            >
              Réinitialiser
            </button>
          </div>

          <div className="text-center" key={mounted ? "client" : "ssr"}>
            <div className="text-3xl md:text-4xl font-extrabold tracking-wide drop-shadow">
              <span suppressHydrationWarning>
                {fmt.format(Math.floor(state.coins))}
              </span>{" "}
              <span className="text-amber-300">SKBC</span>
            </div>
            <div className="text-sm text-slate-400">
              par seconde :{" "}
              <b className="text-slate-200">
                <span suppressHydrationWarning>
                  {mounted ? effectiveCps.toFixed(1) : "0.0"}
                </span>
              </b>
            </div>
            {boost.active && (
              <div className="mt-1 text-xs text-amber-300 font-semibold">
                ⚡ Boost ×{boost.mult}{" "}
                <span className="text-slate-300">
                  (
                  <span suppressHydrationWarning>
                    {Math.ceil((boost.until - now) / 1000)}
                  </span>
                  s)
                </span>
              </div>
            )}
          </div>

          <div className="text-sm text-slate-400">
            Total :{" "}
            <span suppressHydrationWarning>
              {fmt.format(Math.floor(state.totalCoins))}
            </span>
          </div>
        </div>
      </div>

      {/* BONUS COIN — global */}
      {bonusCoin && (
        <button
          onClick={collectBonus}
          className="fixed -translate-x-1/2 -translate-y-1/2 z-[9999] animate-[bounce_1.2s_infinite]"
          style={{
            left: bonusCoin.x,
            top: bonusCoin.y,
            filter:
              bonusCoin.kind === "x5"
                ? "drop-shadow(0 0 26px #f40d30) drop-shadow(0 0 12px #f40d30)"
                : "drop-shadow(0 0 26px #00d062) drop-shadow(0 0 12px #00d062)",
          }}
          aria-label={`Collecter le boost ×${bonusCoin.kind === "x5" ? 5 : 2}`}
          title={`Boost ×${bonusCoin.kind === "x5" ? 5 : 2} pendant 15s`}
        >
          <img
            src={bonusCoin.kind === "x5" ? BONUS_X5_IMG : BONUS_X2_IMG}
            className="w-12"
            alt={`Bonus ×${bonusCoin.kind === "x5" ? 5 : 2}`}
            draggable={false}
          />
        </button>
      )}

      {/* GRID */}
      <div className="max-w-7xl mx-auto grid grid-cols-12 gap-8 px-4 py-8 relative">
        {/* Grosse pièce + succès */}
        <div className="col-span-12 lg:col-span-4">
          {/* Panel Succès */}
          <div
            ref={panelRef}
            className="relative z-[70] rounded-3xl border border-white/15 ring-1 ring-white/10 bg-slate-900/70 backdrop-blur shadow-[0_20px_60px_rgba(0,0,0,.55)] overflow-visible"
          >
            {/* décor */}
            <div className="absolute inset-0 z-0 pointer-events-none rounded-3xl overflow-hidden">
              <div className="absolute inset-0">
                {falling.map((c) => (
                  <div
                    key={c.id}
                    className="absolute -translate-x-1/2 -translate-y-1/2"
                    style={{
                      left: c.x,
                      top: c.y,
                      opacity: c.opacity,
                      filter: "blur(1px)",
                    }}
                  >
                    <CoinVisual
                      style={{ width: c.size, height: c.size }}
                      className="drop-shadow-[0_6px_14px_rgba(0,0,0,.35)]"
                    />
                  </div>
                ))}
              </div>
              <div className="absolute inset-0 h-full bg-gradient-to-b from-transparent via-transparent to-slate-900/95" />
              <div className="absolute inset-0 bg-[radial-gradient(ellipse_at_center,_rgba(251,191,36,0.10),transparent_60%)]" />
            </div>

            {/* Contenu interactif */}
            <div className="relative z-10 flex flex-col items-center py-8 md:py-10">
              <div
                ref={coinWrapRef}
                className="relative w-[340px] h-[340px] select-none"
              >
                <button
                  onClick={(e) => {
                    interactedRef.current = true;
                    handleClick(e);
                  }}
                  className="absolute inset-0 grid place-items-center active:scale-[.97] transition-transform duration-75 outline-none z-20"
                  aria-label="Cliquer pour miner des SKBC"
                >
                  <CoinVisual className="w-[320px] drop-shadow-[0_16px_40px_rgba(245,158,11,.35)]" />
                </button>
                {parts.map((p) => (
                  <span
                    key={p.id}
                    className="pointer-events-none absolute -translate-x-1/2 -translate-y-1/2 text-white font-extrabold text-lg drop-shadow animate-[floatUp_1s_ease-out_forwards] z-30"
                    style={{ left: p.x, top: p.y }}
                  >
                    {p.text}
                  </span>
                ))}
              </div>

              {/* Cartes d’infos */}
              <div className="mt-6 grid grid-cols-3 gap-4 text-center text-sm w-full px-4">
                <div className="px-3 py-2 rounded-xl border border-white/10 bg-white/5">
                  <div className="text-slate-300">Clic</div>
                  <div className="text-amber-300 font-extrabold mt-1 text-base">
                    <span suppressHydrationWarning>
                      {mounted ? pretty(state.clickPower * boostMult) : "—"}
                    </span>
                  </div>
                </div>
                <div className="px-3 py-2 rounded-xl border border-white/10 bg-white/5">
                  <div className="text-slate-300">CPS</div>
                  <div className="text-amber-300 font-extrabold mt-1 text-base">
                    <span suppressHydrationWarning>
                      {mounted ? effectiveCps.toFixed(1) : "0.0"}
                    </span>
                  </div>
                </div>
                <div className="px-3 py-2 rounded-xl border border-white/10 bg-white/5">
                  <div className="text-slate-300">Bâtiments</div>
                  <div className="text-amber-300 font-extrabold mt-1 text-base">
                    <span suppressHydrationWarning>
                      {mounted ? totalBuildings(state) : 0}
                    </span>
                  </div>
                </div>
              </div>

              {/* Succès */}
              <div className="mt-8 w-full px-4">
                <div className="text-slate-300 text-sm mb-2">Succès</div>
                {mounted && (
                  <div className="grid grid-cols-3 sm:grid-cols-4 md:grid-cols-5 lg:grid-cols-6 gap-3">
                    {achs.map((a) => {
                      const unlocked = !!a.unlocked;

                      const base =
                        "relative group rounded-xl border p-2 aspect-square grid place-items-center transition";
                      const unlockedCls =
                        "border-amber-500/60 bg-amber-500/10";
                      const lockedCls =
                        "border-white/10 bg-white/5 [&>*:not(.bt-tip)]:opacity-60";
                      const cls = `${base} ${unlocked ? unlockedCls : lockedCls}`;

                      return (
                        <div
                          key={a.id}
                          className={cls}
                          aria-label={a.name}
                          role="img"
                        >
                          <div
                            className={`${
                              unlocked ? "text-amber-300" : "text-slate-300"
                            } w-8 h-8`}
                          >
                            {unlocked ? (
                              <AchievementIconGraphic
                                iconUrl={a.iconUrl}
                                className="w-full h-full"
                                size={32}
                              />
                            ) : (
                              <LockIcon className="w-8 h-8" />
                            )}
                          </div>

                          {unlocked && (
                            <div className="absolute -top-2 -right-2 bg-amber-500 text-slate-900 rounded-full p-1 shadow">
                              <CheckIcon className="w-3 h-3" />
                            </div>
                          )}

                          {/* Tooltip (toujours 100% d'opacité, non affecté par la carte) */}
                          <div className="bt-tip pointer-events-none absolute left-1/2 -translate-x-1/2 -top-2 -translate-y-full w-56 md:w-64 hidden group-hover:block z-[9999] opacity-100">
                            <div className="rounded-xl border border-white/15 bg-slate-900/95 backdrop-blur px-3 py-2 shadow-2xl">
                              <div className="flex items-start gap-2">
                                <div
                                  className={`${
                                    unlocked
                                      ? "text-amber-300"
                                      : "text-slate-200"
                                  }`}
                                >
                                  {unlocked ? (
                                    <AchievementIconGraphic
                                      iconUrl={a.iconUrl}
                                      className="w-5 h-5"
                                      size={20}
                                    />
                                  ) : (
                                    <LockIcon className="w-5 h-5" />
                                  )}
                                </div>
                                <div className="flex-1">
                                  <div className="text-sm font-semibold">
                                    {a.name}
                                  </div>
                                  <div className="text-xs text-slate-300 mt-0.5">
                                    {a.desc}
                                  </div>
                                </div>
                                <div className="text-xs whitespace-nowrap">
                                  {unlocked ? (
                                    <span className="text-amber-300 font-semibold">
                                      Débloqué ✓
                                    </span>
                                  ) : (
                                    <span className="text-slate-400">
                                      Verrouillé
                                    </span>
                                  )}
                                </div>
                              </div>
                            </div>
                            <div className="mx-auto h-2 w-2 rotate-45 -mt-1 bg-slate-900/95 border-l border-t border-white/15"></div>
                          </div>
                        </div>
                      );
                    })}
                  </div>
                )}
              </div>
            </div>
          </div>
        </div>

        {/* ===================== BOUTIQUE ===================== */}
        <div className="col-span-12 lg:col-span-4">
          <div className="relative z-[60] rounded-3xl border border-white/15 ring-1 ring-white/10 bg-slate-900/70 backdrop-blur shadow-[0_20px_60px_rgba(0,0,0,.55)] overflow-visible">
            <div className="px-5 py-4 border-b border-white/10 text-sm text-slate-300">
              Boutique
            </div>
            {mounted && (
              <div className="divide-y divide-white/5">
                {BUILDINGS.map((b, idx) => {
                  const unlocked = isBuildingUnlocked(state, idx);
                  const owned = state.buildings[b.id] || 0;
                  const cost = nextCost(b.baseCost, owned);
                  const can = state.coins >= cost;
                  const interactable = unlocked && can;

                  const oneCps = perBuildingCps(state, b);
                  const typeCps = oneCps * owned;
                  const share = cps > 0 ? (typeCps / cps) * 100 : 0;

                  return (
                    <button
                      key={b.id}
                      onClick={() => interactable && buyBuilding(b)}
                      disabled={!interactable}
                      className={`w-full relative group px-4 py-3 grid grid-cols-[auto_1fr] gap-4 items-center transition ${
                        interactable
                          ? "hover:bg-white/5"
                          : "cursor-not-allowed [&>*:not(.bt-tip)]:opacity-50"
                      }`}
                      aria-label={
                        unlocked ? `Acheter ${b.name}` : "Bâtiment verrouillé"
                      }
                    >
                      {/* icône */}
                      <div className="w-12 h-12 flex items-center justify-center select-none">
                        {unlocked ? (
                          <img
                            src={b.iconUrl}
                            alt={b.name}
                            className="w-full h-full object-contain"
                            draggable={false}
                          />
                        ) : (
                          <LockIcon className="w-8 h-8 text-slate-300" />
                        )}
                      </div>

                      {/* bloc nom + prix */}
                      <div className="min-w-0 text-left">
                        <div className="font-semibold text-[17px] truncate">
                          {unlocked ? b.name : "???"}
                        </div>
                        <div
                          className={`text-[13px] font-semibold flex items-center gap-1 mt-0.5 ${
                            can ? "text-emerald-300" : "text-red-400"
                          }`}
                        >
                          <CoinInline />
                          <span suppressHydrationWarning>
                            {fmt.format(cost)}
                          </span>
                        </div>
                      </div>

                      {/* compteur à droite (caché si verrouillé) */}
                      {unlocked && (
                        <div className="absolute right-3 top-1/2 -translate-y-1/2 text-4xl md:text-5xl font-black text-slate-300/15 select-none pointer-events-none">
                          <span suppressHydrationWarning>{owned}</span>
                        </div>
                      )}

                      {/* Tooltip — toujours opaque */}
                      <div className="bt-tip pointer-events-none absolute left-2 right-2 -top-2 -translate-y-full hidden group-hover:block z-[9999] opacity-100">
                        <div className="rounded-xl border border-white/15 bg-slate-900/95 backdrop-blur px-3 py-2 shadow-2xl">
                          <div className="flex items-start gap-3">
                            <div className="text-xl select-none">
                              {unlocked ? (
                                <BuildingIconGraphic
                                  b={b}
                                  className="w-5 h-5"
                                  size={20}
                                />
                              ) : (
                                <LockIcon className="w-5 h-5 text-slate-200" />
                              )}
                            </div>
                            <div className="flex-1 min-w-0">
                              <div className="flex items-center gap-2">
                                <div className="text-sm font-semibold">
                                  {unlocked ? b.name : "??? "}
                                </div>
                                {unlocked && (
                                  <span className="text-[10px] px-1.5 py-0.5 rounded bg-white/10 border border-white/15 text-slate-300">
                                    owned:{" "}
                                    <b
                                      className="text-slate-200"
                                      suppressHydrationWarning
                                    >
                                      {owned}
                                    </b>
                                  </span>
                                )}
                              </div>
                            </div>
                            <div className="text-right whitespace-nowrap">
                              <div
                                className={`text-sm font-semibold flex items-center gap-1 justify-end ${
                                  can ? "text-emerald-300" : "text-red-400"
                                }`}
                              >
                                <CoinInline />
                                <span suppressHydrationWarning>
                                  {fmt.format(cost)}
                                </span>
                              </div>
                            </div>
                          </div>

                          {unlocked ? (
                            <>
                              <div className="mt-2 text-xs text-slate-300 italic">
                                « {b.flavor} »
                              </div>
                              <div className="mt-1 space-y-1 text-xs text-slate-200">
                                <div>
                                  • chaque {b.name.toLowerCase()} produit{" "}
                                  <b>{oneCps.toFixed(1)}</b> cps
                                </div>
                                <div>
                                  •{" "}
                                  <span suppressHydrationWarning>{owned}</span>{" "}
                                  {owned <= 1 ? "unité" : "unités"} produisent{" "}
                                  <b suppressHydrationWarning>
                                    {typeCps.toFixed(1)}
                                  </b>{" "}
                                  cps (
                                  <span suppressHydrationWarning>
                                    {share.toFixed(1)}
                                  </span>
                                  % du total)
                                </div>
                              </div>
                              <div
                                className={`mt-2 text-xs ${
                                  can ? "text-amber-300" : "text-slate-400"
                                }`}
                              >
                                {can
                                  ? "Cliquer pour acheter"
                                  : "Fonds insuffisants"}
                              </div>
                            </>
                          ) : (
                            <div className="mt-2 text-xs text-slate-400 italic">
                              « ??? »
                            </div>
                          )}
                        </div>
                        <div className="mx-6 h-2 w-2 rotate-45 -mt-1 bg-slate-900/95 border-l border-t border-white/15"></div>
                      </div>
                    </button>
                  );
                })}
              </div>
            )}
          </div>
        </div>

        {/* Améliorations */}
        <div className="col-span-12 lg:col-span-4">
          <div className="relative z-[60] rounded-3xl border border-white/15 ring-1 ring-white/10 bg-slate-900/70 backdrop-blur shadow-[0_20px_60px_rgba(0,0,0,.55)] overflow-visible">
            <div className="px-5 py-4 border-b border-white/10 text-sm text-slate-300">
              Améliorations
            </div>

            {mounted && (
              <div className="p-4 grid grid-cols-3 sm:grid-cols-4 md:grid-cols-5 lg:grid-cols-6 gap-3">
                {sortedUpgrades.map((u) => {
                  const unlocked = u.available(state);
                  const affordable = state.coins >= u.cost;
                  const purchased = !!u.taken;

                  const base =
                    "relative group rounded-xl border p-2 aspect-square grid place-items-center transition";
                  const purchasedCls =
                    "border-amber-500/60 bg-amber-500/10 cursor-default";
                  const lockedCls =
                    "border-white/10 bg-white/5 cursor-not-allowed [&>*:not(.bt-tip)]:opacity-60";
                  const unaffordableCls =
                    "border-white/10 bg-white/5 cursor-not-allowed [&>*:not(.bt-tip)]:opacity-80";
                  const buyableCls =
                    "border-amber-400/50 bg-amber-400/10 hover:ring-2 hover:ring-amber-400/50 hover:bg-amber-400/20";

                  let cls = base;
                  if (purchased) cls += " " + purchasedCls;
                  else if (!unlocked) cls += " " + lockedCls;
                  else if (unlocked && !affordable) cls += " " + unaffordableCls;
                  else cls += " " + buyableCls;

                  const priceColor = affordable
                    ? "text-emerald-300"
                    : "text-red-400";

                  return (
                    <button
                      key={u.id}
                      onClick={() => buyUpgrade(u)}
                      disabled={purchased || !unlocked || !affordable}
                      className={cls}
                      aria-label={u.name}
                    >
                      <div
                        className={`w-8 h-8 ${
                          purchased
                            ? "text-amber-300"
                            : unlocked
                            ? "text-slate-200"
                            : "text-slate-300"
                        }`}
                      >
                        {unlocked ? (
                          <UpgradeIconGraphic
                            u={u}
                            className={`w-full h-full ${
                              !purchased && unlocked ? "opacity-70" : ""
                            }`}
                            size={32}
                          />
                        ) : (
                          <LockIcon className="w-8 h-8" />
                        )}
                      </div>

                      {purchased && (
                        <div className="absolute -top-2 -right-2 bg-amber-500 text-slate-900 rounded-full p-1 shadow">
                          <CheckIcon className="w-3 h-3" />
                        </div>
                      )}

                      {/* Tooltip — même style, toujours opaque */}
                      <div className="bt-tip pointer-events-none absolute left-1/2 -translate-x-1/2 -top-2 -translate-y-full w-56 md:w-64 z-[9999] hidden group-hover:block opacity-100">
                        <div className="rounded-xl border border-white/15 bg-slate-900/95 backdrop-blur px-3 py-2 shadow-xl">
                          <div className="flex items-start gap-2">
                            <div
                              className={`${
                                purchased ? "text-amber-300" : "text-slate-200"
                              }`}
                            >
                              {unlocked ? (
                                <UpgradeIconGraphic
                                  u={u}
                                  className="w-5 h-5"
                                  size={20}
                                />
                              ) : (
                                <LockIcon className="w-5 h-5" />
                              )}
                            </div>
                            <div className="flex-1">
                              <div className="text-sm font-semibold">
                                {u.name}
                              </div>
                              {(purchased || unlocked) && (
                                <div className="text-xs text-slate-300 mt-0.5">
                                  {u.desc}
                                </div>
                              )}
                            </div>
                            {(purchased || unlocked) && (
                              <div
                                className={`text-xs whitespace-nowrap font-semibold ${priceColor}`}
                              >
                                <span suppressHydrationWarning>
                                  {fmt.format(u.cost)}
                                </span>
                              </div>
                            )}
                          </div>

                          {!unlocked ? (
                            <div className="mt-2 text-xs text-slate-400 italic">
                              Condition : {u.unlockHint || "non remplie"}
                            </div>
                          ) : purchased ? (
                            <div className="mt-2 text-xs text-amber-300">
                              Achetée ✓
                            </div>
                          ) : !affordable ? (
                            <div className="mt-2 text-xs text-slate-400">
                              Prix :{" "}
                              <span suppressHydrationWarning>
                                {formatShort(u.cost)}
                              </span>{" "}
                              — fonds insuffisants
                            </div>
                          ) : (
                            <div className="mt-2 text-xs text-amber-300">
                              Cliquer pour acheter
                            </div>
                          )}
                        </div>
                        <div className="mx-auto h-2 w-2 rotate-45 -mt-1 bg-slate-900/95 border-l border-t border-white/15"></div>
                      </div>
                    </button>
                  );
                })}
              </div>
            )}
          </div>
        </div>
      </div>

      {/* toasts */}
      <div className="fixed left-4 bottom-4 space-y-2 z-[9999]">
        {toasts.map((t) => (
          <div
            key={t.id}
            className="max-w-xs rounded-xl border border-white/10 bg-slate-900/85 px-4 py-3 text-sm shadow-xl animate-[toastIn_.3s_ease-out]"
          >
            <div className="font-medium text-amber-300">{t.title}</div>
            {t.desc ? (
              <div className="text-slate-300 mt-1 text-xs">{t.desc}</div>
            ) : null}
          </div>
        ))}
      </div>

      <style jsx global>{`
        @keyframes floatUp {
          0% {
            transform: translate(-50%, -50%) translateY(0);
            opacity: 1;
          }
          100% {
            transform: translate(-50%, -50%) translateY(-60px);
            opacity: 0;
          }
        }
        @keyframes toastIn {
          from {
            transform: translateY(8px);
            opacity: 0;
          }
          to {
            transform: translateY(0);
            opacity: 1;
          }
        }
        .clamp-3 {
          display: -webkit-box;
          -webkit-line-clamp: 3;
          -webkit-box-orient: vertical;
          overflow: hidden;
        }
      `}</style>
    </div>
  );
}
