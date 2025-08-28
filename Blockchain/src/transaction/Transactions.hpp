#ifndef TRANSACTIONS_HPP
#define TRANSACTIONS_HPP


#include <vector>
#include <set>
#include "Transaction.hpp"

#define MAX_TRANSACTIONS 10


/*Cette object représente un ensemble de transactions à inclure dans un bloc. Les transactions sont sélectionnées en fonction de leur validité et de leur absence d'inputs communs.*/
class BlockTransactions {
private:
    std::vector<const Transaction> txs;


public:

    BlockTransactions(){}
    BlockTransactions(const Blockchain& blockchain, const TransactionsPool& pool, const PubKey& minerPubKey) : txs() {
        double totalFees = 0.0;

        txs.reserve(MAX_TRANSACTIONS + 1);

        for (size_t i = 0; i < txs.size(); ++i) {
            totalFees += txs[i].getFee(blockchain);
            this->txs.push_back(txs[i]);
        }

        txs.push_back(Transaction::miningReward(minerPubKey, totalFees + Blockchain::getMiningRewardAt(blockchain.size())));
    }

    const Transaction& operator[](size_t i) const { return txs[i]; }
    size_t size() const { return txs.size(); }

    double getTotalFees(const Blockchain& blockchain) const {
        double totalFees = 0.0;
        for (size_t i = 0; i < txs.size() - 1; ++i) { // Ignore last tx (mining reward)
            totalFees += txs[i].getFee(blockchain);
        }
        return totalFees;
    }

    static double calculateMinerReward(const Blockchain& blockchain, const Block& block){
        return blockchain.getMiningRewardAt(block.getIndex()) + block.getTransactions().getTotalFees(blockchain);
    }

    bool verify(const Blockchain& blockchain, const Block& block, const UTXOs& unspentOutputs) const {
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

};
#endif // TRANSACTIONS_HPP