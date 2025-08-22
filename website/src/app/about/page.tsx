"use client";

import { motion } from "framer-motion";
import { Download, Pickaxe, Coins, Shield } from "lucide-react";
import Link from "next/link";

export default function AboutPage() {
  return (
    <div className="min-h-screen bg-gradient-to-b from-slate-950 via-slate-900 to-slate-950 text-slate-100">
      {/* HERO */}
      <section className="relative overflow-hidden pt-20 pb-16">
        <h1 className="text-center text-4xl sm:text-5xl md:text-6xl font-extrabold tracking-[0.08em] text-white/90">
          À propos de SKBC
        </h1>
        <p className="mt-6 text-center text-lg text-slate-300 max-w-3xl mx-auto">
          SKBC est une blockchain décentralisée conçue pour être simple, sécurisée et accessible à tous.  
          Découvrez comment fonctionne notre réseau et pourquoi il apporte une nouvelle approche aux
          échanges numériques.
        </p>
      </section>

      {/* SECTION Blockchain */}
      <section className="max-w-5xl mx-auto px-6 py-16 grid md:grid-cols-2 gap-10 items-center">
        <motion.div
          initial={{ opacity: 0, y: 10 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.6 }}
        >
          <h2 className="text-2xl font-bold mb-4">Qu’est-ce qu’une blockchain ?</h2>
          <p className="text-slate-300 leading-relaxed">
            Une blockchain est une base de données distribuée, partagée entre de nombreux ordinateurs.  
            Chaque transaction est validée collectivement par le réseau et ajoutée sous forme de bloc.  
            Grâce à ce mécanisme, aucune entité centrale n’a le contrôle : la confiance vient du consensus.
          </p>
        </motion.div>
        <div className="rounded-2xl border border-white/10 bg-white/5 p-8 text-center">
          <Shield className="h-12 w-12 mx-auto text-emerald-400" />
          <p className="mt-4 text-slate-300">Sécurité et transparence garanties par la cryptographie.</p>
        </div>
      </section>

      {/* SECTION Notre projet */}
      <section className="max-w-5xl mx-auto px-6 py-16 grid md:grid-cols-2 gap-10 items-center">
        <div className="rounded-2xl border border-white/10 bg-white/5 p-8 text-center">
          <Pickaxe className="h-12 w-12 mx-auto text-violet-400" />
          <p className="mt-4 text-slate-300">Un système de minage simple et accessible.</p>
        </div>
        <motion.div
          initial={{ opacity: 0, y: 10 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.6, delay: 0.2 }}
        >
          <h2 className="text-2xl font-bold mb-4">Notre mission</h2>
          <p className="text-slate-300 leading-relaxed">
            Avec SKBC, nous voulons offrir une plateforme où chacun peut participer,  
            miner et sécuriser le réseau, mais aussi échanger des jetons de manière fluide.  
            Notre vision : un écosystème où la technologie blockchain sert directement la communauté.
          </p>
        </motion.div>
      </section>

      {/* SECTION Features */}
      <section className="max-w-6xl mx-auto px-6 py-16 grid sm:grid-cols-3 gap-8">
        <Feature icon={<Pickaxe className="h-6 w-6" />} title="Minage PoW" desc="Chaque utilisateur peut contribuer à la sécurité du réseau." />
        <Feature icon={<Coins className="h-6 w-6" />} title="Transactions rapides" desc="Envoyez et recevez vos SKBC en quelques secondes." />
        <Feature icon={<Shield className="h-6 w-6" />} title="Sécurité" desc="Une technologie robuste basée sur la cryptographie." />
      </section>

      {/* CTA Bottom */}
      <section className="mt-1 flex items-center justify-center gap-4">
        <div className="mt-16 flex items-center justify-center gap-4">
          <Link href="/download" className="inline-flex items-center gap-2 rounded-xl bg-emerald-500 hover:bg-emerald-400 text-slate-950 px-5 py-3 font-medium">
            <Download className="h-5 w-5" /> Télécharger l’app
          </Link>
          <Link href="/" className="inline-flex items-center gap-2 rounded-xl border border-white/15 hover:bg-white/5 px-5 py-3 font-medium text-slate-200"> Retour à l’accueil
          </Link>
        </div>
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

function Feature({ icon, title, desc }: { icon: React.ReactNode; title: string; desc: string }) {
  return (
    <div className="rounded-2xl border border-white/10 bg-white/5 p-5 text-center">
      <div className="h-12 w-12 rounded-xl bg-white/5 border border-white/10 grid place-items-center mx-auto">
        {icon}
      </div>
      <div className="font-medium mt-4">{title}</div>
      <p className="text-slate-300 mt-3 text-sm">{desc}</p>
    </div>
  );
}
