cd Blockchain_desktop_PDG
rm -rf build
cmake -S . -B build -G Ninja
cmake --build build -j
cd build
QML_DISABLE_DISK_CACHE=1 ./appBlockchain_desktop_PDG
