#ifndef BLOCKCHAIN_HPP
#define BLOCKCHAIN_HPP

#include "Block.hpp"
#include "network/NodeNetwork.hpp"
#include "config.hpp"
#include "transaction/TransactionPool.hpp"

#include <thread>
#include <unordered_map>
#include <set>

using UTXOs = std::unordered_map<PubKey, std::set<OutputReference>>;


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

    //Operators
    /*Retourne une référence constante sur le bloc à l'index donné*/
    const Block& operator[](const size_t index) const {return blocks[index];}

    //Setters
    /*Vérifie si le bloc est valide avant de l'ajouter à la blockchain et modifie la liste des sorties non dépensées*/
    bool addBlock(const Block& block);


    //Fonctions
    double getWalletBalance(const PubKey& pubKey) const{
        double balance = 0.0;
        
        // Vérifier si le wallet existe avant d'accéder
        auto it = utxos.find(pubKey);
        if (it != utxos.end()) {
            // Ajouter les sorties non dépensées
            for (const auto& outputRef : it->second) {
                balance += outputRef.getOutput(*this).getValue();
            }
        }
        
        return balance;
    }
    TransactionPool& getTransactionPool() { return transactionPool; }
    const TransactionPool& getTransactionPool() const { return transactionPool; }
    NodeNetwork& getNetwork() { return network; }
    const NodeNetwork& getNetwork() const { return network; }
    const UTXOs& getUTXOs() const { return utxos; }

    void doMine(const PubKey& minerPubKey) {

        isMining = true;

        // Lancer le mining dans un thread séparé pour ne pas bloquer
        std::thread miningThread([this, minerPubKey]() {

            std::cout << "Mining started in background thread." << std::endl;

            while(isMining) {
                // Créer un nouveau bloc avec les transactions en attente
                Block newBlock = Block::createBlock(*this, minerPubKey);

                // Ajouter le bloc à la blockchain
                if (addBlock(newBlock)) {
                    network.buildFrameAndbroadcast(MsgType::BROADCAST_BLOCK, newBlock);
                }
            }

            std::cout << "Mining stopped." << std::endl;
    });
    

    // Détacher le thread pour qu'il s'exécute en arrière-plan
    miningThread.detach();

    }

    void stopMining() {
        isMining = false;
    }


};


#endif //BLOCKCHAIN_HPP