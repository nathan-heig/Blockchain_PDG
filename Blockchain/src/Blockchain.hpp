#ifndef BLOCKCHAIN_HPP
#define BLOCKCHAIN_HPP

#include "Block.hpp"

#include <unordered_map>
#include <set>
#include <deque>

using UTXOs = std::unordered_map<const PubKey, std::set<const OutputReference>>;
using TransactionsPool = std::deque<const Transaction>;

class Blockchain {
private:
    std::vector<const Block> blocks;
    TransactionsPool pendingTransactions;
    UTXOs unspentOutput;

    /*Ajoute une sortie non dépensée à la liste*/
    void addUnspentOutput(const PubKey& pubKey, const OutputReference& outputRef) {unspentOutput[pubKey].insert(outputRef);}
    void deleteUnspentOutput(const PubKey& pubKey, const OutputReference& outputRef) {unspentOutput[pubKey].erase(outputRef);}

public:
    // Constructor
    Blockchain();

    // Getters
    /*Retourne un object Transactions prêt a etre ajouté dans un bloc*/
    const BlockTransactions& getNewBlockTransactions(const PubKey& minerPubKey) const {return BlockTransactions(*this, pendingTransactions, minerPubKey);}
    /*Retourne le mining reward a un index donné*/
    static const double getMiningRewardAt(uint32_t index) {
        // version simplifiée pour l'instant
        return 1000;
    }
    /*Retourne la difficulté à un index donné en se basent sur le temps des blocks precedants l'index*/
    const double getTargetAt(uint32_t index) const{
        // version simplifiée pour l'instant
        return 2;
    }

    //Operators
    /*Retourne une référence constante sur le bloc à l'index donné*/
    const Block& operator[](const size_t index) const {return blocks[index];}
    size_t size() const {return blocks.size();}

    //Setters
    /*Vérifie si le bloc est valide avant de l'ajouter à la blockchain et modifie la liste des sorties non dépensées*/
    bool addBlock(const Block& block) {
        if (!block.verify(*this, unspentOutput)) {
            return false;
        }
        blocks.push_back(block);
        for (size_t i = 0; i < block.getTransactions().size(); ++i) {
            for (size_t j = 0; j < block[i].getOutputs().size(); ++j) {
                const OutputReference outRef(block.getIndex(), i, j);
                addUnspentOutput(block[i].getOutputs()[j].getPubKey(), outRef);
            }
            for (const auto& input : block[i].getInputs()) {
                deleteUnspentOutput(input.getOutput(*this).getPubKey(), input);
            }
        }
        return true;
    }
    /*Vérifie si la transaction est valide avant de l'ajouter à la pool*/
    bool addPendingTransaction(const Transaction& txs) {
        //version simplifiée pour l'instant
        if (!txs.verify(*this, unspentOutput)) {
            return false;
        }
        pendingTransactions.push_back(txs);
        return true;
    }

};


#endif //BLOCKCHAIN_HPP