#ifndef BLOCKCHAIN_HPP
#define BLOCKCHAIN_HPP

#include "Block.hpp"
#include "network/NodeNetwork.hpp"
#include "config.hpp"
#include "transaction/TransactionPool.hpp"

#include <thread>


class Blockchain {
private:
    std::vector<Block> blocks;//vecteur contenant les blocks de la blockchain

    NodeNetwork network{*this};
    TransactionPool transactionPool{*this};//pool de transactions en attente
    UTXOs utxos;//output de transactions non dépensées (unspent transaction outputs)


    bool isMining{false};

    /*Ajoute une sortie non dépensée à la liste*/
    void addUnspentOutput(const PubKey& pubKey, const OutputReference& outputRef) {utxos[pubKey].insert(outputRef);}
    /*Supprime une sortie non dépensée de la liste*/
    void deleteUnspentOutput(const PubKey& pubKey, const OutputReference& outputRef) {utxos[pubKey].erase(outputRef);}

public:


    // Constructor
    Blockchain(){}


    // Getters
    /*Retourne un object Transactions prêt a etre ajouté dans un bloc*/
    const BlockTransactions getNewBlockTransactions(const PubKey& minerPubKey) const {return BlockTransactions(*this, transactionPool, minerPubKey);}
    /*Retourne le mining reward a un index donné*/
    static const double getMiningRewardAt(uint32_t index);
    /*Retourne la difficulté à un index donné en se basent sur le temps des blocks precedants l'index*/
    const Target getTargetAt(uint32_t index) const;
    /*Retourne le nombre de blocs dans la blockchain*/
    uint32_t size() const {return blocks.size();}

    double getWalletBalance(const PubKey& pubKey) const;

    TransactionPool& getTransactionPool() { return transactionPool; }
    const TransactionPool& getTransactionPool() const { return transactionPool; }

    NodeNetwork& getNetwork() { return network; }
    const NodeNetwork& getNetwork() const { return network; }

    const UTXOs& getUTXOs() const { return utxos; }


    //Operators
    /*Retourne une référence constante sur le bloc à l'index donné*/
    const Block& operator[](const size_t index) const {return blocks[index];}


    //Setters
    /*Vérifie si le bloc est valide avant de l'ajouter à la blockchain et modifie la liste des sorties non dépensées*/
    bool addBlock(const Block& block);
    bool addAndBroadCastTransaction(const Transaction& tx);


    // Mining
    void doMine(const PubKey& minerPubKey);
    void stopMining() { isMining = false; }


};


#endif //BLOCKCHAIN_HPP