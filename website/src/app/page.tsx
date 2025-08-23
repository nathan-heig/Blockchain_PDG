"use client";

import React from "react";
import { motion } from "framer-motion";
import { Download, Pickaxe, Coins, Activity, Users, Shield } from "lucide-react";
import Link from "next/link";

// --- Helpers d'affichage ---
const fmt = new Intl.NumberFormat("fr-CH");
const formatHashrate = (h?: number | null) => {
  if (h == null) return "—";
  const units = ["H/s", "kH/s", "MH/s", "GH/s", "TH/s", "PH/s"];
  let i = 0;
  let v = Number(h);
  while (v >= 1000 && i < units.length - 1) {
    v /= 1000;
    i++;
  }
  return `${v.toFixed(2)} ${units[i]}`;
};

// --- Données en dur (modifie ici quand tu veux) ---
type Stats = {
  minersActive: number;
  networkHashrate: number;
  blockHeight: number;
  tps: number;
  balance: number;
};

const STATS: Stats = {
  minersActive: 42,
  networkHashrate: 12_340_000,
  blockHeight: 123_456,
  tps: 3.2,
  balance: 1335.42,
};

const TXS: Array<{ hash: string; delta: number }> = [
  { hash: "0x12ab…89", delta: +12.5 },
  { hash: "0x98ff…aa", delta: -2.0 },
  { hash: "0x77cd…4e", delta: +0.8 },
];

export default function LandingPage() {
  const stats = STATS;

  return (
    <div className="min-h-screen bg-gradient-to-b from-slate-950 via-slate-900 to-slate-950 text-slate-100">
      {/* HERO */}
      <section className="relative overflow-hidden pt-20 mb-16">
        <h1 className="text-center text-4xl sm:text-5xl md:text-6xl font-extrabold tracking-[0.08em] text-white/90">
          SKBC BLOCKCHAIN
        </h1>

        <div className="relative max-w-6xl mx-auto px-6 mt-10">
          <div className="absolute inset-x-0 top-12 mx-auto h-56 sm:h-64 md:h-72 lg:h-80 max-w-5xl rounded-[2.5rem] bg-slate-400/20 blur-[1px]" />

          {/* “Tablette” */}
          <motion.div
            initial={{ opacity: 0, y: 10 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ duration: 0.5 }}
            className="relative mx-auto max-w-5xl rounded-[2rem] border border-black/40 shadow-[0_20px_60px_rgba(0,0,0,0.6)] bg-slate-900/80 backdrop-blur overflow-hidden"
          >
            <div className="grid grid-cols-12 gap-0">
              {/* Colonne Stats */}
              <div className="col-span-3 hidden md:block bg-slate-900/70 border-r border-white/5 p-5">
                <div className="text-slate-300 text-sm font-medium mb-3">Stats</div>
                <StatRow icon={<Users className="h-4 w-4" />} label="Mineurs" value={fmt.format(stats.minersActive)} />
                <StatRow icon={<Activity className="h-4 w-4" />} label="Hashrate" value={formatHashrate(stats.networkHashrate)} />
                <StatRow icon={<Coins className="h-4 w-4" />} label="Bloc" value={`#${fmt.format(stats.blockHeight)}`} />
                <StatRow icon={<Activity className="h-4 w-4" />} label="TPS" value={stats.tps.toFixed(2)} />
              </div>

              {/* Centre “wallet” */}
              <div className="col-span-12 md:col-span-6 p-8 sm:p-10">
                <div className="flex flex-col items-center text-center">
                  <div className="w-8 h-8 rounded-full bg-white/10 grid place-items-center border border-white/20 mb-4">
                    <Shield className="h-4 w-4 text-slate-300" />
                  </div>
                  <div className="text-sm text-slate-400">Available Balance</div>
                  <div className="mt-1 text-4xl font-bold">
                    {stats.balance.toLocaleString("fr-CH", { minimumFractionDigits: 2, maximumFractionDigits: 2 })}
                  </div>
                  <div className="text-xs text-slate-300 mt-1">SKBC</div>

                  <div className="mt-6 w-full max-w-md mx-auto">
                    <div className="rounded-xl bg-slate-500/15 border border-slate-400/30 text-slate-300 px-4 py-2 text-sm">
                      Mining en cours…
                    </div>
                  </div>

                  <div className="mt-6 flex gap-4">
                    <button className="rounded-xl px-5 py-2 bg-slate-500/90 text-white font-medium">Send</button>
                    <button className="rounded-xl px-5 py-2 bg-slate-500/20 text-slate-500 border border-slate-500/40 font-medium">
                      Receive
                    </button>
                  </div>
                </div>
              </div>

              {/* Historique TX */}
              <div className="col-span-3 hidden md:block bg-slate-900/70 border-l border-white/5 p-5">
                <div className="text-slate-300 text-sm font-medium mb-3">Transaction history</div>
                {TXS.map((t) => (
                  <TxItem key={t.hash} hash={t.hash} delta={t.delta} />
                ))}
              </div>
            </div>
          </motion.div>
        </div>

        {/* CTA */}
        <div className="mt-16 flex items-center justify-center gap-4">
          <Link
            href="/download"
            className="inline-flex items-center gap-2 rounded-xl bg-emerald-500 hover:bg-emerald-400 text-slate-950 px-5 py-3 font-medium"
          >
            <Download className="h-5 w-5" /> Télécharger l’app
          </Link>
          <Link
            href="/about"
            className="inline-flex items-center gap-2 rounded-xl border border-white/15 hover:bg-white/5 px-5 py-3 font-medium text-slate-200"
          >
            En savoir plus
          </Link>
        </div>
      </section>

      {/* FEATURES */}
      <section id="features" className="max-w-6xl mx-auto px-6 py-20 grid sm:grid-cols-3 gap-8">
        <Feature icon={<Pickaxe className="h-5 w-5" />} title="Mining PoW" desc="Démarrez un mineur local et obtenez du SKBC." />
        <Feature icon={<Coins className="h-5 w-5" />} title="Wallet Qt" desc="Gérez votre wallet, envoyez/recevez du SKBC." />
        <Feature icon={<Shield className="h-5 w-5" />} title="Signatures" desc="Clés locales, signatures et vérification." />
      </section>

      {/* FOOTER */}
      <footer className="border-t border-white/10 mt-16">
        <div className="max-w-6xl mx-auto px-6 py-10 text-sm text-slate-400 text-center">
          © {new Date().getFullYear()} SKBC Blockchain
        </div>
      </footer>
    </div>
  );
}

// --- Petits composants UI ---
function StatRow({ icon, label, value }: { icon: React.ReactNode; label: string; value: string }) {
  return (
    <div className="flex items-center justify-between py-2 text-sm">
      <div className="flex items-center gap-2 text-slate-400">
        {icon}
        <span>{label}</span>
      </div>
      <div className="text-slate-200 font-medium">{value}</div>
    </div>
  );
}

function TxItem({ hash, delta }: { hash: string; delta: number }) {
  const positive = delta >= 0;
  return (
    <div className="rounded-xl border border-white/10 bg-white/5 p-3 mb-2">
      <div className="text-xs text-slate-400">{hash}</div>
      <div className={`text-sm mt-1 ${positive ? "text-emerald-300" : "text-rose-300"}`}>
        {positive ? "+" : ""}
        {delta.toFixed(2)} SKBC
      </div>
    </div>
  );
}

function Feature({ icon, title, desc }: { icon: React.ReactNode; title: string; desc: string }) {
  return (
    <div className="rounded-2xl border border-white/10 bg-white/5 p-5">
      <div className="flex items-center gap-3">
        <div className="h-9 w-9 rounded-xl bg-white/5 border border-white/10 grid place-items-center">{icon}</div>
        <div className="font-medium">{title}</div>
      </div>
      <p className="text-slate-300 mt-3 text-sm">{desc}</p>
    </div>
  );
}
