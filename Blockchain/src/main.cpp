#include "Blockchain.hpp"
#include "test/crypto.hpp"

int main()
{
    std::cout << "Starting Blockchain node..." << std::endl;
    Blockchain blockchain;

    blockchain.network.start();


    while (true) {
        blockchain.addBlock(Block::createBlock(blockchain, "test"));
        std::cout << "Blockchain size: " << blockchain.size() << " blocks." <<  blockchain.getWalletBalance("test") << std::endl;
    }

    return 0;
}