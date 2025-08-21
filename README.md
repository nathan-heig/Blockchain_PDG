# Build de l'app rapide pour la présentation

## Prérequis
- Qt 6.5
- C++ 17
- Qt6::Quick 
- Qt6::QuickControls2
- Qt6::Qml 
- Qt6::Core5Compat

## Script de build et lancement

### Linux
Le script suivant se lance pour linux depuis le fichier script_pour_presentation.sh
```bash
rm -rf build
cmake -S . -B build -G Ninja
cmake --build build -j
cd build
QML_DISABLE_DISK_CACHE=1 ./appBlockchain_desktop_PDG
```

### MacOS
```bash
rm -rf build Desktop-Debug Desktop-Release 2>/dev/null
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug   # (ou Release)
cmake --build build -j
cd build

# Si ta cible N'EST PAS un bundle macOS (pas de MACOSX_BUNDLE) :
QML_DISABLE_DISK_CACHE=1 ./appBlockchain_desktop_PDG

# Si ta cible EST un bundle macOS :
# add_executable(appBlockchain_desktop_PDG MACOSX_BUNDLE ...)
# alors lance le binaire dans le bundle :
# QML_DISABLE_DISK_CACHE=1 ./appBlockchain_desktop_PDG.app/Contents/MacOS/appBlockchain_desktop_PDG
```

### Windows
Prérequis : Ninja installé, MSVC (vs Build Tools) configuré. A exécuter dans PowerShell.
```bash
# nettoyage
Remove-Item -Recurse -Force build, Desktop-Debug, Desktop-Release -ErrorAction SilentlyContinue

# configure + build
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel

# run (binaire .exe dans build\)
Set-Item Env:QML_DISABLE_DISK_CACHE 1
.\build\appBlockchain_desktop_PDG.exe
```
