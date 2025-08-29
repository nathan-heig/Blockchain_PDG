#include "BlockTransactions.hpp"
#include "Blockchain.hpp"


BlockTransactions::BlockTransactions(const Blockchain& blockchain, const TransactionsPool& pool, const PubKey& minerPubKey) : txs() {
    double totalFees = 0.0;
    txs.reserve(MAX_TRANSACTIONS + 1);

    size_t count = 0;
    for (const auto& tx : pool) {
        if (count >= MAX_TRANSACTIONS) break;
        // TODO: vérifier via interface publique quand disponible
        totalFees += tx.getFee(blockchain);
        txs.push_back(tx);
        ++count;
    }

    txs.push_back(Transaction::miningReward(minerPubKey, totalFees + Blockchain::getMiningRewardAt(blockchain.size())));
}

double BlockTransactions::getTotalFees(const Blockchain& blockchain) const {
    double totalFees = 0.0;
    for (size_t i = 0; i < txs.size() - 1; ++i) { // Ignore last tx (mining reward)
        totalFees += txs[i].getFee(blockchain);
    }
    return totalFees;
}

double BlockTransactions::calculateMinerReward(const Blockchain& blockchain, const Block& block){
    return blockchain.getMiningRewardAt(block.getIndex()) + block.getTransactions().getTotalFees(blockchain);
}

bool BlockTransactions::verify(const Blockchain& blockchain, const Block& block, const UTXOs& unspentOutputs) const {
    for (size_t i = 0; i < txs.size() - 1; ++i) { // Ignore last tx (mining reward)
        if (!txs[i].verify(blockchain, unspentOutputs)) {
            return false;
        }
    }
    if (!txs.back().verifyMiningReward(blockchain, block)) {
        return false;
    }
    return true;
}