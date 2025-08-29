#ifndef TRANSACTIONS_HPP
#define TRANSACTIONS_HPP


#include <vector>
#include <set>
#include "Transaction.hpp"

#include <cereal/types/vector.hpp>

#define MAX_TRANSACTIONS 10

using TransactionsPool = std::set<Transaction>;

/*Cette object représente un ensemble de transactions à inclure dans un bloc. Elle est responsable de verifier la validité des transactions et de gerer les récompense de minage.*/
class BlockTransactions {
private:
    std::vector<Transaction> txs;

public:

    BlockTransactions() = default;
    BlockTransactions(const Blockchain& blockchain, const TransactionsPool& pool, const PubKey& minerPubKey);

    static double calculateMinerReward(const Blockchain& blockchain, const Block& block);

    //Operator
    const Transaction& operator[](size_t i) const { return txs[i]; }

    //Getters
    size_t size() const { return txs.size(); }
    double getTotalFees(const Blockchain& blockchain) const;

    bool verify(const Blockchain& blockchain, const Block& block, const UTXOs& unspentOutputs) const;

    template<class Archive>
    void serialize(Archive& ar){
        ar(txs);
    }

};
#endif // TRANSACTIONS_HPP