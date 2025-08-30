#include "TransactionPool.hpp"
#include "Blockchain.hpp"





const Transaction Transaction::create(EVP_PKEY* fromPrivKey, const PubKey& toPubKey, double amount, double fee, const Blockchain& blockchain){

    const PubKey fromPubKey = crypto::getPubKey(fromPrivKey);

    if (blockchain.getWalletBalance(fromPubKey) < amount + fee) {
        throw std::runtime_error("Insufficient balance");
    }

    Inputs inputs;
    double totalBalance = 0.0;
    for( OutputReference outRef : blockchain.getUTXOs().at(fromPubKey)) {
        inputs.push_back(outRef);
        totalBalance += outRef.getOutput(blockchain).getValue();
        if (totalBalance >= amount + fee) break;
    }

    Outputs outputs;
    outputs.push_back(Output(amount, toPubKey));
    double change = totalBalance - amount - fee;
    if (change > 0) {
        outputs.push_back(Output(change, fromPubKey));
    }

    auto tx = Transaction(inputs, outputs);
    tx.sign(fromPrivKey);
    return tx;
}


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