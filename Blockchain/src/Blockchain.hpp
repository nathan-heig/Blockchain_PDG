#ifndef BLOCKCHAIN_HPP
#define BLOCKCHAIN_HPP

#include "Block.hpp"
#include "network/NodeNetwork.hpp"

#include <unordered_map>
#include <set>

using UTXOs = std::unordered_map<PubKey, std::set<OutputReference>>;

//en attendant d'avoir une classe pour ca
using TransactionsPool = std::set<Transaction>;

class Blockchain {
private:
    std::vector<Block> blocks;//vecteur contenant les blocks de la blockchain
    TransactionsPool pendingTransactions;//pool de transactions en attente
    UTXOs utxos;//output de transactions non dépensées (unspent transaction outputs)

    /*Ajoute une sortie non dépensée à la liste*/
    void addUnspentOutput(const PubKey& pubKey, const OutputReference& outputRef) {utxos[pubKey].insert(outputRef);}
    /*Supprime une sortie non dépensée de la liste*/
    void deleteUnspentOutput(const PubKey& pubKey, const OutputReference& outputRef) {utxos[pubKey].erase(outputRef);}

public:

    NodeNetwork network{*this};
    // Constructor
    Blockchain(){}

    // Getters
    /*Retourne un object Transactions prêt a etre ajouté dans un bloc*/
    const BlockTransactions getNewBlockTransactions(const PubKey& minerPubKey) const {return BlockTransactions(*this, pendingTransactions, minerPubKey);}
    /*Retourne le mining reward a un index donné*/
    static const double getMiningRewardAt(uint32_t index);
    /*Retourne la difficulté à un index donné en se basent sur le temps des blocks precedants l'index*/
    const double getTargetAt(uint32_t index) const;
    /*Retourne le nombre de blocs dans la blockchain*/
    size_t size() const {return blocks.size();}

    //Operators
    /*Retourne une référence constante sur le bloc à l'index donné*/
    const Block& operator[](const size_t index) const {return blocks[index];}
    Block& operator[](const size_t index) {return blocks[index];}

    //Setters
    /*Vérifie si le bloc est valide avant de l'ajouter à la blockchain et modifie la liste des sorties non dépensées*/
    bool addBlock(const Block& block);
    /*Vérifie si la transaction est valide avant de l'ajouter à la pool*/
    bool addPendingTransaction(const Transaction& tx);


    //Fonctions


};


#endif //BLOCKCHAIN_HPP