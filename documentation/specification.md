# SkibidiCoin (SKIBI)

## Description
Le monde des cryptomonnaies est fascinant mais semble peu accessible. Étant donné que dans notre groupe nous sommes majoritairement en option sécurité et sommes intéressés par la cryptographie, nous avons choisi d'explorer le fonctionnement et les détails d'une blockchain. 
L'idée clé de ce projet est donc de créer une cryptomonnaie à but pédagogique, volontairement simplifiée, qui nous permettrait d'apprendre, d'expérimenter et de voir fonctionner une blockchain de bout en bout, mais aussi de créer une interface utilisateur simple et claire. 
Pour cela, nous mettrons en place: 
- Consensus Proof of Work inspiré du Bitcoin
- Transactions signées avec ECDSA
- Réseau P2P
- Wallet desktop pour miner la cryptomonnaie, manipuler les clés et avoir une vue d'ensemble sur les transactions 
- Site web de présentation et de téléchargement de l'application desktop.

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
- Documentation
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
### Backend desktop:
- Application C++ avec Qt
- Challenge cryptographique proof of work
- Algorithme de hash: SHA-256
- OpenSSL pour signatures ECDSA
- Difficulté ajustable pour maintenir un temps de résolution par bloc stable
- Framework networking: Socket BSD pour le P2P, Boost.asio pour gérer la partie asynchrone
- Récompense par bloc pour le mineur: à définir ultérieurement
- Temps par bloc: 1min - 5min
- Taille par bloc: ~ 100 à 500 transactions
- C++
- Framework networking: Socket BSD pour le P2P, Boost.asio pour gérer la partie asynchrone
- Sérialisation

### Interface desktop:
- Wallet intégré avec import/export de clés
- Gestionnaire de transactions avec historique
- Qt 6.x bridge avec Figma

### Site web:
- Landpage design
- Téléchargement de l'app desktop
- guide d'installation et documentation (peut-être à double avec github)
- FAQ trollesque
- React.js avec TypeScript
- Téléchargement de l'app desktop
- guide d'installation et documentation
- Hébergé chez Nathan

### Idées supplémentaires:
- ThreeJS pour le site web
- plugin Qt pour un meilleur design
- Protection DDOS
- Ajouter des statistiques sur la blockchain dans le site web


