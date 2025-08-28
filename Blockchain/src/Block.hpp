#ifndef BLOCK_HPP
#define BLOCK_HPP


#include "Transactions.hpp"

class Blockchain;


class Block {
private:
    // Attributs
    uint32_t index;
    uint32_t nonce;

    uint32_t timestamp;
    uint8_t target;

    BlockTransactions transactions;

    Hash previousHash;
    Hash hash;

    // Fonctions
    Hash calculateHash() const {return crpto::hashData(std::to_string(index) + std::to_string(nonce));}//version test
    const bool hashMatchesDifficulty() const;

    Block(){}

public:

    /*Crée un nouveau bloc à partir de la blockchain et de la clé publique du mineur*/
    static Block createBlock(const Blockchain& blockchain, const PubKey& minerPubKey);

    // Operators
    const Transaction& operator[](const size_t i) const {return transactions[i];}


    // Getters
    const uint32_t getIndex() const { return index; }
    const uint32_t getTimestamp() const { return timestamp; }
    const Hash& getPreviousHash() const { return previousHash; }
    const Hash& getHash() const { return hash; }
    const BlockTransactions& getTransactions() const {return transactions;}

    /*Cette fonction vérifie la validité du bloc en s'assurant que le hash correspond à la difficulté et que les transactions sont valides.*/
    bool verify(const Blockchain& blockchain, const UTXOs& utxos) const;

};

#endif //BLOCKCHAIN_CLASS_HPP