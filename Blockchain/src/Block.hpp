#ifndef BLOCK_HPP
#define BLOCK_HPP


#include "Transaction.hpp"

class Block {
private:
    // Attributs
    uint32_t index;
    uint32_t nonce;

    uint32_t timestamp;
    std::vector<Transaction> transactions;
    std::string previousHash;
    std::string hash;

    // Fonctions
    std::string calculateHash() const;

public:

    // Constructors
    Block(){}

    // Operators
    const Transaction& operator[](size_t i) const {return transactions[i];}



    // Getters
    const uint32_t getIndex() const { return index; }
    const uint32_t getTimestamp() const { return timestamp; }
    const std::string& getPreviousHash() const { return previousHash; }
    const std::vector<Transaction>& getTransactions() const {return transactions;}
    const std::string& getHash() const { return hash; }
};

#endif //BLOCKCHAIN_CLASS_HPP