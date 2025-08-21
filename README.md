# Build de l'app rapide pour la présentation

## Prérequis
- Qt 6.5
- C++ 17
- Qt6::Quick 
- Qt6::QuickControls2
- Qt6::Qml 
- Qt6::Core5Compat

## Script de build et lancement
Le script suivant se lance correctement depuis le fichier script_pour_presentation.sh

```bash
rm -rf build
cmake -S . -B build -G Ninja
cmake --build build -j
cd build
QML_DISABLE_DISK_CACHE=1 ./appBlockchain_desktop_PDG
```

