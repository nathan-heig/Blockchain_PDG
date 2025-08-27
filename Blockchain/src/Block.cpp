#include "Block.hpp"
#include "Blockchain.hpp"


Block Block::createBlock(const Blockchain& blockchain, const PubKey& minerPubKey) {
    Block block;
    block.index = blockchain.getBlocks().size();
    block.timestamp = static_cast<uint32_t>(time(nullptr));
    block.previousHash = blockchain.getBlocks().back().getHash();
    block.target = blockchain.getTargetAt(block.index);

    // Add pending transactions
    double fees;
    block.transactions = blockchain.getPendingTransactions(10, fees);
    double reward = blockchain.getMiningRewardAt(block.index) + fees;

    block.transactions.push_back(Transaction::miningReward(minerPubKey, reward));

    // Proof of Work
    block.nonce = 0;
    do {
        ++block.nonce;
        block.hash = block.calculateHash();
    } while (block.hashMatchesDifficulty());

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