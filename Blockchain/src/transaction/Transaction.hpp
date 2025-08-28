#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <iostream>
#include <sstream>
#include <cstdint>
#include <vector>
#include <string>
#include "cryptography/crypto.hpp"
#include "OutputReference.hpp"

class Blockchain;
class Block;
class UTXOs;

#define MAX_INPUTS 100
#define MAX_OUTPUTS 50



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
    const bool verifySignature(const Blockchain& blockchain) const {return crpto::verifySignature(getStrToSign(), signature, inputs[0].getOutput(blockchain).getPubKey());}

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
    void sign(EVP_PKEY* privateKey) {signature = crpto::signData(this->getStrToSign(), privateKey);}

    /*Vérifie la validité de la transaction et ne valide pas une récompense de minage*/
    const bool verify(const Blockchain& blockchain, const UTXOs& unspentOutputs) const;
    /*Vérifie une récompense de minage*/
    const bool verifyMiningReward(const Blockchain& blockchain, const Block& block) const;
};


#endif // TRANSACTION_HPP