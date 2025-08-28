#include "Block.hpp"
#include "Blockchain.hpp"


Block Block::createBlock(const Blockchain& blockchain, const PubKey& minerPubKey) {
    Block block;
    block.index = blockchain.size();
    block.timestamp = static_cast<uint32_t>(time(nullptr));
    block.previousHash = blockchain[block.index - 1].getHash();
    block.target = blockchain.getTargetAt(block.index);

    block.transactions = blockchain.getNewBlockTransactions(minerPubKey);

    // Proof of Work
    block.nonce = 0;
    do {
        ++block.nonce;
        block.hash = block.calculateHash();
    } while (!block.hashMatchesDifficulty());

    return block;
}


const bool Block::hashMatchesDifficulty() const {
    for (uint8_t i = 0; i < target; ++i) {
        if (hash[i] != '0') {
            return false;
        }
    }
    return true;
}