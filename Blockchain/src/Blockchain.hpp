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
    UTXOs unspentOutputReferences;

    /*Ajoute une sortie non dépensée à la liste*/
    void addUnspentOutput(const PubKey& pubKey, const OutputReference& outputRef) {unspentOutputReferences[pubKey].insert(outputRef);}

public:
    // Constructor
    Blockchain();

    // Getters
    /*Retourne la pool de transactions en attente*/
    const TransactionsPool& getPendingTransactions() const { return pendingTransactions; }
    /*Retourne le mining reward a un index donné*/
    static const double getMiningRewardAt(uint32_t index) {return 1000;}
    /*Retourne la difficulté à un index donné en se basent sur le temps des blocks precedants l'index*/
    const double getTargetAt(uint32_t index) const{return 2;}
    /*Retourne la liste des sorties non dépensées*/
    const UTXOs& getUnspentOutputReferences() const {return unspentOutputReferences;}
    /*Retourne la liste des blocs*/
    const std::vector<const Block>& getBlocks() const { return blocks; }

    //Iterator
    /*Retourne un itérateur constant sur le début des blocs*/
    std::vector<const Block>::const_iterator cbegin() const {return blocks.cbegin();}
    /*Retourne un itérateur constant sur la fin des blocs*/
    std::vector<const Block>::const_iterator cend() const {return blocks.cend();}

    //Operators
    /*Retourne une référence constante sur le bloc à l'index donné*/
    const Block& operator[](const size_t index) const {return blocks[index];}

    //Setters
    /*Vérifie si le bloc est valide avant de l'ajouter à la blockchain et modifie la liste des sorties non dépensées*/
    void addBlock(const Block& block);
    /*Vérifie si la transaction est valide avant de l'ajouter à la pool*/
    void addPendingTransaction(const Transaction& tx);
};


#endif //BLOCKCHAIN_HPP