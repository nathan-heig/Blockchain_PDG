#include "Blockchain.hpp"

int main()
{
    Blockchain blockchain;

    for(int i = 0; i < 5; ++i) {

        // Create a new block
        Block block = Block::createBlock(blockchain, "test");

        // Add the block to the blockchain
        blockchain.addBlock(block);
    }
    return 0;
}