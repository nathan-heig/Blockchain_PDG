#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <iostream>
#include <sstream>
#include <cstdint>
#include <vector>
#include <string>
#include "cryptography/key.hpp"

class Blockchain; // Forward declaration to avoid circular dependency

class Output {
private:
    double value;
    PubKey pubKey;

public:
    Output(double value, const PubKey& pubKey)
        : value(value), pubKey(pubKey) {}

    const double getValue() const { return value; }
    const PubKey& getPubKey() const { return pubKey; }

    const std::string toString() const {
        std::ostringstream oss;
        oss << "Value: " << value << ", PubKey: " << pubKey;
        return oss.str();
    }
};
using Outputs = std::vector<const Output>;

class OutputReference {
private:
    uint32_t blockIndex;
    uint16_t txIndex;
    uint16_t outputIndex;

public:
    //Constructor
    OutputReference(uint32_t blockIdx, uint16_t txIdx, uint16_t outIdx)
        : blockIndex(blockIdx), txIndex(txIdx), outputIndex(outIdx) {}
    

    //Comparison operator
    bool operator<(const OutputReference& other) const {
        return std::tie(blockIndex, txIndex, outputIndex) < std::tie(other.blockIndex, other.txIndex, other.outputIndex);
    }


    //Getters
    const Output& getOutput(const Blockchain& blockchain) const;

    //String representation
    const std::string toString() const {
        std::ostringstream oss;
        oss << "Block: " << blockIndex << ", Tx: " << txIndex << ", Out: " << outputIndex;
        return oss.str();
    }

};
using Inputs = std::vector<const OutputReference>;

class Transaction {
private:
    Inputs inputs;
    Outputs outputs;
    Signature signature;

    //Verification methods
    /*Vérifie les entrées de la transaction*/
    const bool verifyInputs(const Blockchain& blockchain, const UTXOs& unspentOutputs) const;
    /*Vérifie les sorties de la transaction*/
    const bool verifyOutputs() const;
    /*Vérifie que la transaction est solvable*/
    const bool verifySold(const Blockchain& blockchain) const {return getFee(blockchain) >= 0;}
    /*Vérifie la signature de la transaction*/
    const bool verifySignature(const Blockchain& blockchain) const {return key::verifySignature(getStrToSign(), signature, inputs[0].getOutput(blockchain).getPubKey());}

public:
    //Constructor
    Transaction(const Inputs&& inputs, Outputs&& outputs)
        : inputs(std::move(inputs)), outputs(std::move(outputs)), signature() {}

    /*Crééer une transaction de récompense de minage*/
    static const Transaction miningReward(const PubKey& minerPubKey, const double reward) {
        Output rewardOutput(reward, minerPubKey);
        return Transaction({}, {rewardOutput});
    }

    //Getters
    const Inputs& getInputs() const { return inputs; }
    const Outputs& getOutputs() const { return outputs; }
    const double getFee(const Blockchain& blockchain) const;
    const std::string getStrToSign() const;

    //Signature methods
    void sign(EVP_PKEY* privateKey) {signature = key::signData(this->getStrToSign(), privateKey);}

    /*Vérifie la validité de la transaction et ne valide pas une récompense de minage*/
    const bool verify(const Blockchain& blockchain, const UTXOs& unspentOutputs) const;
    /*Vérifie une récompense de minage*/
    const bool verifyMiningReward(const Blockchain& blockchain, const Block& block) const;
};


#endif // TRANSACTION_HPP