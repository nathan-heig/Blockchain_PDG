#include "Block.hpp"
#include "Blockchain.hpp"


Block Block::createBlock(const Blockchain& blockchain, const PubKey& minerPubKey, const std::atomic<bool>* continueFlag) {


    Block block;
    block.index = blockchain.size();
    block.timestamp = static_cast<uint32_t>(time(nullptr));
    block.previousHash = blockchain.size() > 0 ? blockchain[blockchain.size() - 1].getHash() : "0";
    block.target = blockchain.getTargetAt(block.index);

    block.transactions = blockchain.getNewBlockTransactions(minerPubKey);

    // Proof of Work
    block.nonce = 0;
    do {
        ++block.nonce;
        block.hash = block.calculateHash();
    } while (!block.hashMatchesDifficulty() && (block.index == blockchain.size()) && continueFlag->load());

    return block;
}

const bool Block::hashMatchesDifficulty() const {
    for (uint8_t i = 0; i < target.value; ++i) {
        if (hash[i] != 0) {
            return false;
        }
    }
    if (static_cast<uint8_t>(hash[target.value]) > target.max) {
        return false;
    }
    return true;
}

bool Block::verify(const Blockchain& blockchain, const UTXOs& utxos) const {
    if(index == 0) return true;
    return index == blockchain.size() && (calculateHash() == hash) && hashMatchesDifficulty() && transactions.verify(blockchain, *this, utxos) && (index == 0 || previousHash == blockchain[index - 1].getHash());
}