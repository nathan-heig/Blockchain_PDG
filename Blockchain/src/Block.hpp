#ifndef BLOCK_HPP
#define BLOCK_HPP


#include "Transaction.hpp"

class Block {
private:
    // Attributs
    uint32_t index;
    uint32_t nonce;

    uint32_t timestamp;
    uint8_t target;
    std::vector<const Transaction> transactions;
    Hash previousHash;


    Hash hash;

    // Fonctions
    Hash calculateHash() const;

public:

    // Constructors
    Block(){}
    Block(uint32_t idx, uint32_t time, uint8_t tgt, const Hash& prevHash, std::vector<const Transaction>&& txs)
        : index(idx), nonce(0), timestamp(time), target(tgt), transactions(std::move(txs)), previousHash(prevHash), hash() {}

    // Operators
    const Transaction& operator[](const size_t i) const {return transactions[i];}


    // Getters
    const uint32_t getIndex() const { return index; }
    const uint32_t getTimestamp() const { return timestamp; }
    const Hash& getPreviousHash() const { return previousHash; }
    const Hash& getHash() const { return hash; }
    const std::vector<const Transaction>& getTransactions() const {return transactions;}
};

#endif //BLOCKCHAIN_CLASS_HPP