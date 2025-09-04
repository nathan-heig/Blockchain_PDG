#ifndef TRANSACTION_POOL_HPP
#define TRANSACTION_POOL_HPP

#include "Transaction.hpp"
#include <set>

/**
 * Classe représentant le pool de transactions en attente.
 * Elle assure la gestion des transactions en attente et leur validation.
 */
class TransactionPool {
private:
    const Blockchain& blockchain_;

    std::set<Transaction> transactions_;
    std::set<OutputReference> spentOutputs_;
    mutable std::mutex mutex_;

public:

    TransactionPool(const Blockchain& blockchain) : blockchain_(blockchain) {}

    bool addTransaction(const Transaction& tx);
    bool removeTransaction(const Transaction& tx);


    const std::set<Transaction>& getTransactions() const{return transactions_;}

};

#endif // TRANSACTION_POOL_HPP