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
    std::lock_guard<std::mutex> lk(mtx_);
    if (blocks.empty() || index == 0) {
        return Target::createInitialTarget();
    }

    const uint32_t era = index / 10;

    if (era == 0) {
        return blocks.front().getTarget();
    }

    uint32_t start = (era - 1) * 10;
    if (blocks.size() <= start) {
        // Pas assez d'historique pour calculer proprement
        return Target::createInitialTarget();
    }
    uint32_t end = era * 10 - 1;
    uint32_t last = static_cast<uint32_t>(blocks.size() - 1);
    if (end > last) end = last;  // tronque si la fenêtre n'est pas encore complète

    // Moyenne des écarts temporels
    uint64_t timeDiffSum = 0;
    uint32_t count = 0;
    for (uint32_t j = start + 1; j <= end; ++j) {
        timeDiffSum += blocks[j].getTimestamp() - blocks[j - 1].getTimestamp();
        ++count;
    }

    // S'il n'y a pas encore de pairs on conserve la cible
    if (count == 0) {
        return blocks[start].getTarget();
    }

    const uint32_t avgTime = static_cast<uint32_t>(timeDiffSum / count);

    // Applique l'ajustement à partir de la cible
    Target t = blocks[start].getTarget();
    if (avgTime < 60 * 4) {        // plus rapide que 4 min -> +difficulté
        t.augmenterDifficulte(1);
    } else if (avgTime > 60 * 6) { // plus lent que 6 min  -> -difficulté
        t.diminuerDifficulte(1);
    }
    return t;
}



bool Blockchain::addBlock(const Block& block) {
    std::lock_guard<std::mutex> lk(mtx_);
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
    std::lock_guard<std::mutex> lk(mtx_);
    double balance = 0.0;
    
    // Vérifier si le wallet existe avant d'accéder
    auto it = utxos.find(pubKey);
    if (it != utxos.end()) {
        // Ajouter les sorties non dépensées
        for (const auto& ref : it->second) {
            const auto& tx = blocks[ref.getBlockIndex()][ref.getTxIndex()]; // accès interne sous lock
            balance += tx.getOutputs()[ref.getOutputIndex()].getValue();
        }
    }
    
    return balance;
}

UTXOs Blockchain::getUTXOsSnapshot() const {
    std::lock_guard<std::mutex> lk(mtx_);
    return utxos; // copie
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

        while(isMining.load()) {
            auto start = std::chrono::steady_clock::now();
            // Créer un nouveau bloc avec les transactions en attente
            Block newBlock = Block::createBlock(*this, minerPubKey);
            auto end = std::chrono::steady_clock::now();

            bool accepted = false;
            {
                if (addBlock(newBlock)) accepted = true;
            }

            if (accepted) {
                // hashrate approx = nonce / durée
                double secs = std::chrono::duration<double>(end - start).count();
                if (secs > 0.0) {
                    double mh = (double)newBlock.getNonce() / secs / 1e6;
                    lastHashrateMHs.store(mh);
                }
                // callback ui (reward)
                double reward = BlockTransactions::calculateMinerReward(*this, newBlock);
                if (onBlockMined) {
                    try { onBlockMined(reward); } catch(...) {}
                }
                // broadcast hors lock
                network.buildFrameAndbroadcast(MsgType::BROADCAST_BLOCK, newBlock);
            }

            // broadcast hors lock
            network.buildFrameAndbroadcast(MsgType::BROADCAST_BLOCK, newBlock);
        }

        std::cout << "Mining stopped." << std::endl;
});


// Détacher le thread pour qu'il s'exécute en arrière-plan
miningThread.detach();

}