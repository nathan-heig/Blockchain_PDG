#include "Blockchain.hpp"


const double Blockchain::getMiningRewardAt(uint32_t index) {
    uint32_t nbHalvings = index / 10000;
    if (nbHalvings >= 64) {
        return 0.0;
    }
    return 100 / (1 << nbHalvings);
    //supply max 2 millions de coin comme ca (environ 35 jours pas halving si block 5 min)
}

const Target Blockchain::getTargetAt(uint32_t index) const {

    if(index >= blocks.size()) {
        throw std::out_of_range("Invalid block index");
    }

    uint32_t test = index / 10;

    if (test <= 0){
        return Target::createInitialTarget();
    }

    uint64_t timeDiffSum = 0;
    for(uint32_t i = (test -1) * 10; i < test * 10; ++i){
        timeDiffSum += blocks[i+1].getTimestamp() - blocks[i].getTimestamp();
    }
    uint32_t avgTime = timeDiffSum / 10;

    if (avgTime < 60*4) {// si plus petit que 4 minutes
        Target t = blocks[(test - 1) * 10].getTarget();
        t.augmenterDifficulte(1);
        return t;
    } else if (avgTime > 60*6) {// si plus grand que 6 minutes
        Target t = blocks[(test -1) * 10].getTarget();
        t.diminuerDifficulte(1);
        return t;
    } else {// si entre 4 et 6 minutes
        return blocks[(test - 1) * 10].getTarget();
    }
}


bool Blockchain::addBlock(const Block& block) {
    if (!block.verify(*this, utxos)) {
        return false;
    }

    blocks.push_back(block);
    for (size_t i = 0; i < block.getBlockTransactions().size(); ++i) {

        //Supprime la transaction de la pool
        transactionPool.removeTransaction(block.getBlockTransactions()[i]);

        //itere sur les sortie pour les ajouter aux unspentoutputs
        for (size_t j = 0; j < block[i].getOutputs().size(); ++j) {
            const OutputReference outRef(block.getIndex(), i, j);
            addUnspentOutput(block[i].getOutputs()[j].getPubKey(), outRef);
        }

        //itere sur les entrées pour les supprimer des unspentoutputs
        for (const auto& input : block[i].getInputs()) {
            deleteUnspentOutput(input.getOutput(*this).getPubKey(), input);
        }
    }
    return true;
}

double Blockchain::getWalletBalance(const PubKey& pubKey) const{
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



bool Blockchain::addAndBroadCastTransaction(const Transaction& tx) {

    if (transactionPool.addTransaction(tx)) {
        network.buildFrameAndbroadcast(MsgType::BROADCAST_TX, tx);
        return true;
    }
    return false;
}

void Blockchain::doMine(const PubKey& minerPubKey) {

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