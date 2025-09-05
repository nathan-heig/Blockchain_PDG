# Blochain Desktop PDG
Application de portefeuille de cryptomonnaie développée avec Qt6 et C++17 pour la gestion de transactions et le mining SKBC.
## Fonctionnalités
- Gestion de portefeuille (visualisation du solde et de l'historique des transactions)
- Mining
- Stats en temps réel
- Architecture modulaire avec séparation entre le backend c++ et le frontend QML

## Prérequis

### Dépendances système
- Qt 6.5 ou supérieur (testé avec Qt 6.8.3)
- Compilateur C++17
    - Linux: GCC 9+ ou Clang 10+
    - Windows: MSVC 2022
    - MacOS: Xcode 13+ avec Clang
- CMake 3.16 ou supérieur
- Ninja (pour build plus rapidement)

### Modules Qt requis
- Qt6::Core
- Qt6::Quick
- Qt6::QuickControls2
- Qt6::Qml
- Qt6::Core5Compat
- Qt6::Test (pour les tests unitaires)

## Structure de projet
```text
src/
├── Blockchain.cpp/hpp     # Logique de la blockchain
├── Block.cpp/hpp          # Structure des block et minage
├── cryptography/          # Logique cryptographique
├── network/               # P2P networking
├── transaction/           # Gestion des transaction 
└── ui/                    # Interface Qt/QML

qml/
├── Main.qml              # UI principale
├── components/           # Composants UI
└── styles/               # Thème et style

```
## Script de build et lancement

### Linux
```bash
cd Blockchain_desktop_PDG
rm -rf build
mkdir build && cd build
cmake ..
make -j$(nproc)
./appBlockchain_desktop_PDG
```

### MacOS
```bash
cd Blockchain_desktop_PDG
rm -rf build
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel

# Pour un bundle Macos:
open appBlockchain_desktop_PDG.app

# Pour lancer directement:
./appBlockchain_desktop_PDG.app/Contents/MacOS/appBlockchain_desktop_PDG

# En cas de problème de cache QML:
QML_DISABLE_CACHE=1 ./appBlockchain_desktop_PDG.app/Contents/MacOS/appBlockchain_desktop_PDG
```

### Windows
```bash
cd Blockchain_desktop_PDG
# Nettoyage
Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue

# Configuration et build
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel

# Lancement
cd build
.\appBlockchain_desktop_PDG.exe

# Si problème de cache QML:
$env:QML_DISABLE_DISK_CACHE = "1"
.\appBlockchain_desktop_PDG.exe
```

## Tests unitaires
Lancer l'exécutable de chaque test dans le dossier build après la compilation

## Problèmes courants et solutions
|Problème|Solution|
|--------|--------|
| Qt modules not found | Vérifier CMAKE_PREFIX_PATH ou installer les modules Qt manquants
| QML file not found | Vérifier que resources.qrc est bien compilé dans CMakeLists.txt |
| Linking errors Windows | Installer les Visual C++ Redistributables |
| Bundle macOS ne se lance pas | Utiliser macdeployqt pour déployer les dépendances |

## Contribution
Veuillez consulter le [guide de contribution](CONTRIBUTING.md) pour plus de détails
