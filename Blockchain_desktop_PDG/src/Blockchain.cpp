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
    //std::lock_guard<std::mutex> lk(mtx_);
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

    //Nouveau bloc accepté (hors lock)
    if (onNewBlock) {
        try { onNewBlock(block); } catch(...) {}
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

bool Blockchain::addAndBroadCastTransaction(const Transaction& tx) {

    if (transactionPool.addTransaction(tx)) {
        network.buildFrameAndbroadcast(MsgType::BROADCAST_TX, tx);
        return true;
    }
    return false;
}

void Blockchain::doMine(const PubKey& minerPubKey) {

    isMining_.store(true);


    // Lancer le mining dans un thread séparé pour ne pas bloquer
    std::thread miningThread([this, minerPubKey]() {

        std::cout << "Mining started in background thread." << std::endl;
        const double alpha = 0.3;

        while(isMining_) {
            auto start = std::chrono::steady_clock::now();
            // Créer un nouveau bloc avec les transactions en attente
            Block newBlock = Block::createBlock(*this, minerPubKey, &isMining_, &lastHashrateMHs);

            //std::cout << "Mining new block at index " << newBlock.getIndex() << " with target " << (int)newBlock.getTarget().value << std::endl;
            auto end = std::chrono::steady_clock::now();

            bool accepted = addBlock(newBlock);

            if (accepted) {

                // TPS EMA (recalcule tps immédiat sur fenêtre)
                {
                    std::lock_guard<std::mutex> lk(mtx_);
                    double tps_inst = computeTPS_NoLock(10);
                    double prev = lastTPS_;
                    double ema = alpha * tps_inst + (1.0 - alpha) * prev;
                    lastTPS_ = ema;
                }

                // broadcast réseau
                network.buildFrameAndbroadcast(MsgType::BROADCAST_BLOCK, newBlock);
            }
        }
        std::cout << "Mining stopped." << std::endl;
});


// Détacher le thread pour qu'il s'exécute en arrière-plan
miningThread.detach();

}

double Blockchain::computeTPS_NoLock(uint32_t window) const {
    if (blocks.size() < 2) return 0.0;

    uint32_t endIdx = static_cast<uint32_t>(blocks.size() - 1);
    uint32_t startIdx = (endIdx >= window ? endIdx - window + 1 : 0);

    // Durée entre le 1er et le dernier bloc de la fenêtre
    uint32_t t0 = blocks[startIdx].getTimestamp();
    uint32_t t1 = blocks[endIdx].getTimestamp();
    if (t1 <= t0) return 0.0;
    double duration = static_cast<double>(t1 - t0);

    // Nombre de tx confirmées (hors coinbase) sur la fenêtre
    uint64_t txCount = 0;
    for (uint32_t i = startIdx; i <= endIdx; ++i) {
        const auto& bt = blocks[i].getBlockTransactions();
        if (bt.size() > 0) {
            // coinbase est la dernière → hors coinbase = size()-1 si size>0
            txCount += (bt.size() >= 1 ? (bt.size() - 1) : 0);
        }
    }
    return txCount / duration;
}
