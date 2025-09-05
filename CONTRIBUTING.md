# Contribuer à SkibidiCoin

## Prérequis
- C++20 compiler
- Qt 6.4+ avec Quick, QuickControls2, Qml, Core5Compat
- CMake 3.16+
- Boost.System
- OpenSSL
- miniupnpc
- Git

## build (Linux)

```bash
git clone https://github.com/nathan-heig/Blockchain_PDG.git
cd Blockchain_desktop_PDG
mkdir build && cd build
cmake ..
make -j$(nproc)
./appBlockchain_desktop_PDG
```

## Dépendances
### Ubuntu/Debian
```bash
sudo apt-get install qt6-base-dev qt6-declarative-dev qt6-quickcontrols2-dev \
                     libboost-system-dev libssl-dev libminiupnpc-dev
```

### macOS
```bash
brew install qt@6 boost openssl miniupnpc
```

### Windows (vcpkg)
```bash
vcpkg install qt6-base qt6-declarative qt6-quickcontrols2 boost-system openssl miniupnpc
```

## Workflow de contribution
```text
1. Fork et clone
   ↓
2. Créer une branche
   ↓
3. Développer et commiter
   ↓
4. Push et pull request sur main
   ↓
5. Review et merge
```

### Synchronisation
```bash
git fetch upstream
git rebase upstream/main
```

## Pipeline CI/CD
Selon les branches:

- dev_app
    * Actions on push: CI/build-test
- dev_web
    * Actions on push: PR validation
- main
    * Actions on merge: CI / PR validation / Déploiement

## Testing
```bash
cd build
ctest --output-on-failure
```

### Ecrire des tests
Placer les tests dans `tests/` et utiliser le format Qt Test.  
Exemple:
```bash
class TestTransaction : public QObject {
    Q_OBJECT
private slots:
    void testValidTransaction() {
        // Action
        Transaction tx = createTestTransaction();
        
        // Vérification
        bool isValid = tx.verify(blockchain, utxos);
        
        // Assert
        QVERIFY(isValid);
    }
};
```