#ifndef TRANSACTIONS_HPP
#define TRANSACTIONS_HPP


#include <vector>
#include <set>
#include "Transaction.hpp"

#define MAX_TRANSACTIONS 10

class TransactionsPool;


/*Cette object représente un ensemble de transactions à inclure dans un bloc. Elle est responsable de verifier la validité des transactions et de gerer les récompense de minage.*/
class BlockTransactions {
private:
    std::vector<const Transaction> txs;

public:

    BlockTransactions(){}
    BlockTransactions(const Blockchain& blockchain, const TransactionsPool& pool, const PubKey& minerPubKey);

    static double calculateMinerReward(const Blockchain& blockchain, const Block& block);

    //Operator
    const Transaction& operator[](size_t i) const { return txs[i]; }

    //Getters
    size_t size() const { return txs.size(); }
    double getTotalFees(const Blockchain& blockchain) const;

    bool verify(const Blockchain& blockchain, const Block& block, const UTXOs& unspentOutputs) const;

};
#endif // TRANSACTIONS_HPP