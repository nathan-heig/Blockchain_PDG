#include "Blockchain.hpp"
#include "test/crypto.hpp"

int main()
{
    std::cout << "Starting Blockchain node..." << std::endl;
    Blockchain blockchain;

    blockchain.getNetwork().start();

    blockchain.getNetwork().connect(PeerInfo("127.0.0.1", 8186));

    blockchain.doMine("miner2");

    while (true) {
        std::cout << "Blockchain size: " << blockchain.size() << " blocks." << std::endl << blockchain.getWalletBalance("miner1") << ":" << blockchain.getWalletBalance("miner2") << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(4));
    }

    return 0;
}