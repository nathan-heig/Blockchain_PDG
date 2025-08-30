#include "Blockchain.hpp"
#include "test/crypto.hpp"

int main()
{
    std::cout << "Starting Blockchain node..." << std::endl;
    Blockchain blockchain;

    blockchain.network.start();

    blockchain.network.connect(PeerInfo("127.0.0.1", 8185));

    while (true) {
        std::cout << "Blockchain size: " << blockchain.size() << " blocks." << std::endl; // <<  blockchain.getWalletBalance("test") << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(4));
        //blockchain.addBlock(Block::createBlock(blockchain, "miner1"));
    }

    return 0;
}