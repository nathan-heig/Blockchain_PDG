#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <iostream>
#include <sstream>
#include <cstdint>
#include <vector>
#include <string>
#include "key.hpp"

class Blockchain; // Forward declaration to avoid circular dependency

class Output {
private:
    double value;
    PubKey pubKey;

public:
    Output(double value, const PubKey& pubKey)
        : value(value), pubKey(pubKey) {}

    double getValue() const { return value; }
    const PubKey& getPubKey() const { return pubKey; }

    const std::string toString() const {
        std::ostringstream oss;
        oss << "Value: " << value << ", PubKey: " << pubKey;
        return oss.str();
    }
};

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

class Transaction {
private:
    const std::vector<OutputReference> inputs;
    const std::vector<Output> outputs;
    Signature signature;

public:
    //Constructor
    Transaction(const std::vector<OutputReference>&& inputs,
                        const std::vector<Output>&& outputs)
        : inputs(std::move(inputs)), outputs(std::move(outputs)), signature() {}

    
    //Getters
    const std::vector<OutputReference>& getInputs() const { return inputs; }
    const std::vector<Output>& getOutputs() const { return outputs; }
    const double getFee(const Blockchain& blockchain) const;
    const std::string getStrToSign() const;

    //Signature methods
    void sign(EVP_PKEY* privateKey) {signature = key::signData(this->getStrToSign(), privateKey);}
    bool verify(const Blockchain& blockchain) const;
};


#endif // TRANSACTION_HPP