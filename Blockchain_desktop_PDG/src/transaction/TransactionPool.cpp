#include "TransactionPool.hpp"
#include "Blockchain.hpp"


const Transaction Transaction::create(EVP_PKEY* fromPrivKey, const PubKey& toPubKey,
                                      double amount, double fee, const Blockchain& blockchain)
{
    const PubKey fromPubKey = crypto::getPubKey(fromPrivKey);

    if (blockchain.getWalletBalance(fromPubKey) < amount + fee) {
        throw std::runtime_error("Insufficient balance");
    }

    Inputs inputs;
    double totalBalance = 0.0;

    // Utiliser find() plutôt que at()
    const auto allUtxos = blockchain.getUTXOsSnapshot();
    auto it = allUtxos.find(fromPubKey);
    if (it == allUtxos.end() || it->second.empty()) {
        throw std::runtime_error("No UTXOs available for sender");
    }

    for (const auto& outRef : it->second) {
        inputs.push_back(outRef);
        totalBalance += outRef.getOutput(blockchain).getValue();
        if (totalBalance >= amount + fee) break;
    }

    if (totalBalance < amount + fee) {
        throw std::runtime_error("Insufficient gathered UTXOs");
    }

    Outputs outputs;
    outputs.push_back(Output(amount, toPubKey));
    double change = totalBalance - amount - fee;
    if (change > 0) {
        outputs.push_back(Output(change, fromPubKey));
    }

    Transaction tx(inputs, outputs);
    tx.sign(fromPrivKey);
    return tx;
}



bool TransactionPool::addTransaction(const Transaction& tx){
    try {
        auto utxoSnap = blockchain_.getUTXOsSnapshot();
        if (!tx.verify(blockchain_, utxoSnap)) {
            return false;
        }
    } catch (...) {
        return false; // jamais de crash : on refuse simplement
    }

    std::lock_guard<std::mutex> lock(mutex_);

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