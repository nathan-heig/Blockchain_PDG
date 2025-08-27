#ifndef BLOCK_HPP
#define BLOCK_HPP


#include "Transaction.hpp"

class Blockchain;

class Block {
private:
    // Attributs
    uint32_t index;
    uint32_t nonce;

    time_t timestamp;
    uint8_t target;
    std::vector<const Transaction> transactions;
    Hash previousHash;


    Hash hash;

    // Fonctions
    Hash calculateHash() const;
    const bool hashMatchesDifficulty() const;

public:

    // Constructors
    Block(){}
    static Block createBlock(const Blockchain& blockchain, const PubKey& minerPubKey);

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