#include "Transaction.hpp"
#include "Blockchain.hpp"

const Output& OutputReference::getOutput(const Blockchain& blockchain) const {
    return blockchain[blockIndex][txIndex].getOutputs()[outputIndex];
}

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
    if (inputs.size() > 100 or inputs.empty()){
        return false; // A transaction must have at least one input and no more than 100
    }

    const PubKey pubKey = this->inputs[0].getOutput(blockchain).getPubKey();
    for(const auto& input : inputs) {
        const Output& out = input.getOutput(blockchain);
        if (out.getValue() <= 0) {
            return false; // Output value must be positive
        }
        if (out.getPubKey() != pubKey) {
            return false; // All inputs must be from the same owner
        }
        if (unspentOutputs.at(pubKey).find(input) == unspentOutputs.at(pubKey).end()) {
            return false; // Input must be unspent
        }
    }
    return true;
}

const bool Transaction::verifyOutputs() const {
    if (outputs.size() > 100 or outputs.empty()){
        return false; // A transaction must have at least one output and no more than 100
    }

    for (const auto& output : outputs) {
        if (output.getValue() <= 0) {
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
