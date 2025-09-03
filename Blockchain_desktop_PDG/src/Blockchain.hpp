#ifndef BLOCKCHAIN_HPP
#define BLOCKCHAIN_HPP

#include "Block.hpp"
#include "network/NodeNetwork.hpp"
#include "config.hpp"
#include "transaction/TransactionPool.hpp"

#include <mutex>
#include <thread>
#include <atomic>
#include <functional>

class Blockchain {
private:
    std::vector<Block> blocks;//vecteur contenant les blocks de la blockchain

    NodeNetwork network{*this};
    TransactionPool transactionPool{*this};//pool de transactions en attente
    UTXOs utxos;//output de transactions non dépensées (unspent transaction outputs)

    mutable std::mutex mtx_; //protège blocks / utxos / transactionPool
    std::atomic<bool> isMining_{false};
    std::atomic<double> lastHashrateMHs{0.0}; //pour les stats
    std::atomic<double> lastTPS_{0.0};

    std::function<void(const Block&)> onNewBlock; // nouveau bloc accepté (local ou réseau)

    /*Ajoute une sortie non dépensée à la liste*/
    void addUnspentOutput(const PubKey& pubKey, const OutputReference& outputRef) {utxos[pubKey].insert(outputRef);}
    /*Supprime une sortie non dépensée de la liste*/
    void deleteUnspentOutput(const PubKey& pubKey, const OutputReference& outputRef) {utxos[pubKey].erase(outputRef);}

    double computeTPS_NoLock(uint32_t window = 10) const;
public:


    // Constructor
    Blockchain(){}

    /*Retourne le nombre de blocs dans la blockchain*/
    uint32_t size() const { std::lock_guard<std::mutex> lk(mtx_); return (uint32_t)blocks.size(); }
    double getWalletBalance(const PubKey& pubKey) const;
    /*Retourne une référence constante sur le bloc à l'index donné*/
    const Block& operator[](const size_t index) const { std::lock_guard<std::mutex> lk(mtx_); return blocks[index]; }


    // Getters
    /*Retourne un object Transactions prêt a etre ajouté dans un bloc*/
    const BlockTransactions getNewBlockTransactions(const PubKey& minerPubKey) const {return BlockTransactions(*this, transactionPool, minerPubKey);}
    /*Retourne le mining reward a un index donné*/
    static const double getMiningRewardAt(uint32_t index);
    /*Retourne la difficulté à un index donné en se basent sur le temps des blocks precedants l'index*/
    const Target getTargetAt(uint32_t index) const;

    TransactionPool& getTransactionPool() { return transactionPool; }
    const TransactionPool& getTransactionPool() const { return transactionPool; }

    NodeNetwork& getNetwork() { return network; }
    const NodeNetwork& getNetwork() const { return network; }

    const UTXOs& getUTXOs() const { return utxos; }

    bool isMining() const { return isMining_.load(); }



    //Setters
    /*Vérifie si le bloc est valide avant de l'ajouter à la blockchain et modifie la liste des sorties non dépensées*/
    bool addBlock(const Block& block);
    bool addAndBroadCastTransaction(const Transaction& tx);
    /**
     * Définit le callback à appeler lorsqu'un nouveau bloc est accepté.
    */
    void setOnNewBlock(const std::function<void(const Block&)>& callback) { onNewBlock = callback; }


    // Mining
    void doMine(const PubKey& minerPubKey);
    void stopMining() { isMining_.store(false); }


};


#endif //BLOCKCHAIN_HPP