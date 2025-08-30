#include "Blockchain.hpp"


const double Blockchain::getMiningRewardAt(uint32_t index) {
    // version simplifiée pour l'instant
    return 1000;
}

const Target Blockchain::getTargetAt(uint32_t index) const {
    // version simplifiée pour l'instant
    return Target{2, 32};
}


bool Blockchain::addBlock(const Block& block) {
    if (!block.verify(*this, utxos)) {
        return false;
    }

    blocks.push_back(block);
    for (size_t i = 0; i < block.getBlockTransactions().size(); ++i) {

        //Supprime la transaction de la pool
    // pendingTransactions stocke des copies; tentative d'effacement par equivalence
    pendingTransactions.erase(block.getBlockTransactions()[i]);

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


bool Blockchain::addPendingTransaction(const Transaction& tx) {
    if (!tx.verify(*this, utxos)) {
        return false;
    }
    pendingTransactions.insert(tx);
    return true;
}