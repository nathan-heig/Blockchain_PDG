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

bool Transaction::verify(const Blockchain& blockchain) const {
    if (this->inputs.size() >= 256 or this->outputs.size() >= 256 or signature.empty()) {
        return false;
    }

    PubKey pubKey = this->inputs[0].getOutput(blockchain).getPubKey();
    for (const auto& input : inputs) {
        if (input.getOutput(blockchain).getPubKey() != pubKey) {
            return false; // All inputs must come from the same public key
        }
    }

    return key::verifySignature(this->getStrToSign(), signature, pubKey);
}
