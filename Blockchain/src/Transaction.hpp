#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <iostream>
#include <sstream>
#include <cstdint>
#include <vector>
#include <string>
#include "key.hpp"

class BlockChain; // Forward declaration to avoid circular dependency

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
    OutputReference(uint32_t blockIdx, uint16_t txIdx, uint16_t outIdx)
        : blockIndex(blockIdx), txIndex(txIdx), outputIndex(outIdx) {}
    
    bool operator<(const OutputReference& other) const {
        return std::tie(blockIndex, txIndex, outputIndex) < std::tie(other.blockIndex, other.txIndex, other.outputIndex);
    }

    const Output& getOutput(const BlockChain& blockchain) const;

    const std::string toString() const {
        std::ostringstream oss;
        oss << "Block: " << blockIndex << ", Tx: " << txIndex << ", Out: " << outputIndex;
        return oss.str();
    }
};

class UnsignedTransaction {
private:
    const std::vector<OutputReference> inputs;
    const std::vector<Output> outputs;

public:
    UnsignedTransaction(const std::vector<OutputReference>&& inputs,
                        const std::vector<Output>&& outputs)
        : inputs(std::move(inputs)), outputs(std::move(outputs)) {}

    const std::vector<OutputReference>& getInputs() const { return inputs; }
    const std::vector<Output>& getOutputs() const { return outputs; }

    const std::string toString() const {
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

    const double getFee() const {
        double inputSum = 0.0;
        double outputSum = 0.0;
        for (const auto& output : outputs) {
            outputSum += output.getValue();
        }
        return inputSum - outputSum;
    }
};

class Transaction : public UnsignedTransaction {
private:
    Signatures signatures;

public:
    Transaction(const UnsignedTransaction&& utx, const Signatures&& sigs)
        : UnsignedTransaction(std::move(utx)), signatures(std::move(sigs)) {}

    const Signatures& getSignatures() const { return signatures; }

    struct CompareByFees {
        bool operator()(const Transaction& a, const Transaction& b) const {
            return a.getFee() < b.getFee();
        }
    };

};

#endif // TRANSACTION_HPP