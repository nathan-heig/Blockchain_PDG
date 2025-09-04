#include "Block.hpp"
#include "Blockchain.hpp"


Block Block::createBlock(const Blockchain& blockchain, const PubKey& minerPubKey, const std::atomic<bool>* continueFlag, double* hashrateMHs) {


    Block block;
    block.index = blockchain.size();
    block.timestamp = static_cast<uint32_t>(time(nullptr));
    block.previousHash = blockchain.size() > 0 ? blockchain[blockchain.size() - 1].getHash() : "0";
    block.target = blockchain.getTargetAt(block.index);

    block.transactions = blockchain.getNewBlockTransactions(minerPubKey);

    // Proof of Work
    block.nonce = 0;
    auto startTime = std::chrono::high_resolution_clock::now();

    do {
        ++block.nonce;
        block.hash = block.calculateHash();

        // Mettre à jour le hashrate toutes les 65536 itérations pour ne pas impacter les performances
        if ((block.nonce & 0xFFFF) == 0 && hashrateMHs) {
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - startTime).count();
            if (duration > 0) {
                // (nonce / duration_us) * 1'000'000 = H/s. On divise par 1'000'000 pour avoir des MH/s.
                // Donc MH/s = nonce / duration_us
                *hashrateMHs = static_cast<double>(block.nonce) / duration;
            }
        }
    } while (!block.hashMatchesDifficulty() && (block.index == blockchain.size()) && continueFlag->load());

    // Calcul final du hashrate pour la précision
    if (hashrateMHs) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        if (totalDuration > 0) {
            *hashrateMHs = static_cast<double>(block.nonce) / totalDuration;
        }
    }

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