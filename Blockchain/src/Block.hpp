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
    Hash calculateHash() const;
    const bool hashMatchesDifficulty() const;

    Block(){}

public:

    // Constructors
    static Block createBlock(const Blockchain& blockchain, const PubKey& minerPubKey);

    // Operators
    const Transaction& operator[](const size_t i) const {return transactions[i];}


    // Getters
    const uint32_t getIndex() const { return index; }
    const uint32_t getTimestamp() const { return timestamp; }
    const Hash& getPreviousHash() const { return previousHash; }
    const Hash& getHash() const { return hash; }
    const BlockTransactions& getTransactions() const {return transactions;}


    bool verify(const Blockchain& blockchain, const UTXOs& unspentOutputs) const {
        return (calculateHash() == hash) && hashMatchesDifficulty() && transactions.verify(blockchain, *this, unspentOutputs) && (index == 0 || previousHash == blockchain[index - 1].getHash());
    }

};

#endif //BLOCKCHAIN_CLASS_HPP