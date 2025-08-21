# SkibidiCoin (SKIBI)

## Description
Le but du projet est de créer une cryptomonnaie experimentale reposant sur une blockchain peer-to-peer avec un mécanisme cryptographique Proof of Work (PoW) basé sur l'algorithme de hachage SHA-256. Le projet offre:
- Un backend blockchain (noeuds, minage, transactions)
- Une application desktop avec wallet intégré pour gérer les transactions et le minage.
- Une landing page web de présentation intégrant une documentation et un lien de téléchargement de l'application desktop (Windows, MacOS et Linux).

### Exigences fonctionnelles
1. Blockchain
- Génération et validations de blocs par un mécanisme PoW
- Ajustement automatique de la difficulté pour maintenir un temps de résolution de bloc d'environ 1min.
- Limitation d'un nombre maximal de tokens (x Millions)

2. Transactions
- Création et transmission des transactions signées avec ECDSA
- Vérification des signatures
- Historique local des transactions à jour 

3. Interface desktop
- Génération/import/export de clés
- Affichage du solde et de l'historique des transactions
- Envoi et réception des coins
- implémentation via Qt6 à partir de mockups Figma

4. Site web
- présentation du projet
- Téléchargement de l'application desktop
- Documentation et FAQ
- implémentation en React et Typescript

### Exigences non-fonctionnelles
1. Performances
- Temps cible par bloc court (1min - 5min) pour la démonstration

2. Sécurité
- Signatures ECDSA pour toutes les transactions

3. Portabilité
- Binaire disponibles pour Windows, MacOS et Linux

4. Simplicité d'utilisation
- interface utilisateur ergonomique

5. Déploiement
- CI/CD automatisé avec GitHub Actions

## Spécificités techniques
## Backend desktop:
- Challenge cryptographique proof of work
- Algorithme de hash: SHA-256
- OpenSSL pour signatures ECDSA
- Difficulté ajustable pour maintenir un temps de résolution par bloc stable
- Temps par bloc: 1min - 5min
- Taille par bloc: ~ 100 à 500 transactions
- C++
- Framework networking ?? (Socket BSD pour du p2p ??, Boost.asio pour gérer asynchrone)
- Sérialisation
- Récompense par bloc pour le mineur

## Interface desktop:
- Wallet intégré avec import/export de clés
- Gestionnaire de transactions avec historique
- Console de deboggage ?
- Qt 6.x bridge avec Figma

## Site web:
- Landpage design
- stats Real time
- Téléchargement de l'app desktop
- guide d'installation et documentation (peut-être à double avec github)
- FAQ trollesque
- React.js avec TypeScript
- Hébergé chez Nathan

## Idées supplémentaires:
- ThreeJS pour le site web
- plugin Qt pour un meilleur design
- Protection DDOS
- Ajouter des statistiques sur la blockchain dans le site web


