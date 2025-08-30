#include "TransactionPool.hpp"
#include "Blockchain.hpp"


bool TransactionPool::addTransaction(const Transaction& tx){
    if (!tx.verify(blockchain_, blockchain_.getUTXOs())) { // vérification basique
        return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    // Vérifie que les entrées ne sont pas déjà utilisées dans la pool
    for (const auto& input : tx.getInputs()) {
        if (spentOutputs_.count(input) > 0) {
            return false;
        }
        spentOutputs_.insert(input);
    }
    transactions_.insert(tx);
    return true;
}

bool TransactionPool::removeTransaction(const Transaction& tx){
    std::lock_guard<std::mutex> lock(mutex_);
    // Vérifie que la transaction existe dans la pool
    if (transactions_.erase(tx) > 0) {
        // Supprime les sorties dépensées
        for (const auto& input : tx.getInputs()) {
            spentOutputs_.erase(input);
        }
        return true;
    }
    return false;
}