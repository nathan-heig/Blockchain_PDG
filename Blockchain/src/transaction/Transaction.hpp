#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <iostream>
#include <sstream>
#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include <set>

#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

#include "test/crypto.hpp"
#include "OutputReference.hpp"

class Blockchain;
class Block;
using UTXOs = std::unordered_map<PubKey, std::set<OutputReference>>;

#define MAX_INPUTS 200
#define MAX_OUTPUTS 20



/*Cette classe représente une transaction dans la blockchain. Elle contient des entrées (inputs) et des sorties (outputs), ainsi qu'une signature pour vérifier l'authenticité de la transaction.*/
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
    const bool verifySignature(const Blockchain& blockchain) const {return crypto::verifySignature(getStrToSign(), signature, inputs[0].getOutput(blockchain).getPubKey());}

public:
    //Constructors
    Transaction() = default; // pour désérialisation
    Transaction(Inputs inputsIn, Outputs outputsIn)
        : inputs(std::move(inputsIn)), outputs(std::move(outputsIn)), signature() {}

    /*Crééer une transaction de récompense de minage*/
    static const Transaction miningReward(const PubKey& minerPubKey, const double reward) {
        Output rewardOutput(reward, minerPubKey);
        return Transaction({}, {rewardOutput});
    }
    /*Créer une transaction signée*/
    static const Transaction create(EVP_PKEY* fromPrivKey, const PubKey& toPubKey, double amount, double fee, const Blockchain& blockchain);

    bool operator<(const Transaction& other) const {
        return std::tie(inputs, signature) < std::tie(other.inputs, other.signature);
    }
    //Getters
    const Inputs& getInputs() const { return inputs; }
    const Outputs& getOutputs() const { return outputs; }
    const double getFee(const Blockchain& blockchain) const;
    const std::string getStrToSign() const;

    //Signature methods
    void sign(EVP_PKEY* privateKey) {signature = crypto::signData(this->getStrToSign(), privateKey);}

    /*Vérifie la validité de la transaction et ne valide pas une récompense de minage*/
    const bool verify(const Blockchain& blockchain, const UTXOs& unspentOutputs) const;
    /*Vérifie une récompense de minage*/
    const bool verifyMiningReward(const Blockchain& blockchain, const Block& block) const;

    template<class Archive>
    void serialize(Archive& ar){
        // signature après inputs/outputs
        ar(inputs, outputs, signature);
    }
};


#endif // TRANSACTION_HPP