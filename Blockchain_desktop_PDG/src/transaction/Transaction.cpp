#include "Transaction.hpp"
#include "Blockchain.hpp"


const double Transaction::getFee(const Blockchain& blockchain) const {
    double inputSum = 0.0;
    double outputSum = 0.0;

    for (const auto& input : inputs) {
        inputSum += input.getOutput(blockchain).getValue();
    }
    for (const auto& output : outputs) {
        outputSum += output.getValue();
    }
    return inputSum - outputSum;
}

const std::string Transaction::getStrToSign() const {
        std::ostringstream oss;
        oss << "Inputs:\n";
        for (const auto& input : inputs) {
            oss << "  " << input.toString() << "\n";
        }
        oss << "Outputs:\n";
        for (const auto& output : outputs) {
            oss << "  " << output.toString() << "\n";
        }
        return oss.str();
    }


const bool Transaction::verifyInputs(const Blockchain& blockchain, const UTXOs& unspentOutputs) const {
    if (inputs.empty() || inputs.size() >= MAX_INPUTS) {
        return false;
    }

    // PubKey de référence: propriétaire du premier input
    const PubKey pubKey = inputs[0].getOutput(blockchain).getPubKey();

    // Vérifie que la clé existe dans UTXOs
    auto it = unspentOutputs.find(pubKey);
    if (it == unspentOutputs.end()) {
        return false;
    }
    const auto& ownedUtxos = it->second;

    for (const auto& input : inputs) {
        const Output& out = input.getOutput(blockchain);
        if (out.getValue() <= 0) {
            return false;
        }
        if (out.getPubKey() != pubKey) {
            return false; // tous les inputs doivent appartenir au même owner
        }
        if (ownedUtxos.find(input) == ownedUtxos.end()) {
            return false; // l’input doit être non dépensé
        }
    }
    return true;
}


const bool Transaction::verifyOutputs() const {
    if (outputs.size() >= MAX_OUTPUTS or outputs.empty()){
        return false; // A transaction must have at least one output and no more than MAX_OUTPUTS
    }

    for (const auto& output : outputs) {
        if (output.getValue() < 0) {
            return false; // Output value must be positive
        }
    }
    return true;
}

const bool Transaction::verify(const Blockchain& blockchain, const UTXOs& unspentOutputs) const {
    return verifyInputs(blockchain, unspentOutputs) && verifyOutputs() && verifySold(blockchain) && verifySignature(blockchain);
}

const bool Transaction::verifyMiningReward(const Blockchain& blockchain, const Block& block) const {
    return outputs.size() == 1 and inputs.empty() and signature.empty() and outputs[0].getValue() == BlockTransactions::calculateMinerReward(blockchain, block);
}

const Transaction Transaction::createWithFromPub(EVP_PKEY* fromPrivKey,
                                                 const PubKey& fromPubKey,
                                                 const PubKey& toPubKey,
                                                 double amount, double fee,
                                                 const Blockchain& blockchain)
{
    if (blockchain.getWalletBalance(fromPubKey) < amount + fee) {
        throw std::runtime_error("Insufficient balance");
    }

    Inputs inputs;
    double totalBalance = 0.0;

    const auto allUtxos = blockchain.getUTXOs();
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
    const double change = totalBalance - amount - fee;
    if (change > 0) {
        outputs.push_back(Output(change, fromPubKey));
    }

    Transaction tx(inputs, outputs);
    tx.sign(fromPrivKey);
    return tx;
}


std::string Transaction::getTransactionWalletStr(const PubKey& pubKey, const Blockchain& blockchain) const{
    double amount = 0.0;

    for (const auto& input : inputs) {
        if (input.getOutput(blockchain).getPubKey() == pubKey) {
            amount -= input.getOutput(blockchain).getValue();
        }
    }

    for (const auto& output : outputs) {
        if (output.getPubKey() == pubKey) {
            amount += output.getValue();
        }
    }

    // Helper lambda pour formater les pubKeys
    auto formatPubKey = [](const std::string& key) -> std::string {
        if (key.length() <= 16) return key;
        return key.substr(0, 8) + "..." + key.substr(key.length() - 8);
    };

    if (amount < 0) {
        return "de " + formatPubKey(pubKey) + "\nà " + formatPubKey(outputs[0].getPubKey()) + "\n" + std::to_string(amount);
    } else {
        if (inputs.size() > 0) {
            return "de " + formatPubKey(inputs[0].getOutput(blockchain).getPubKey()) + "\nà " + formatPubKey(pubKey) + "\n" + std::to_string(amount);
        }
        return "de Mining reward\nà " + formatPubKey(pubKey) + "\n" + std::to_string(amount);
    }
}