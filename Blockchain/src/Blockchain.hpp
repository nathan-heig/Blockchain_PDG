#ifndef BLOCKCHAIN_HPP
#define BLOCKCHAIN_HPP

#include "Block.hpp"

#include <unordered_map>
#include <set>
#include <deque>

class Blockchain {
private:
    std::vector<const Block> blocks;
    std::deque<const Transaction> pendingTransactions;
    std::unordered_map<PubKey, std::set<OutputReference>> unspentOutputReferences;

    void addUnspentOutput(const PubKey& pubKey, const OutputReference& outputRef) {unspentOutputReferences[pubKey].insert(outputRef);}

public:
    // Constructor
    Blockchain() {}

    // Getters
    /*
    Cette fonction retourne les n premieres transactions
    */
    const std::vector<const Transaction&>& getPendingTransactions(int number) const;

    //Iterator
    std::vector<const Block>::const_iterator cbegin() const {return blocks.cbegin();}
    std::vector<const Block>::const_iterator cend() const {return blocks.cend();}

    //Operators
    const Block& operator[](size_t index) const {return blocks[index];}

    //Setters
    void addBlock(const Block& block);
        void addPendingTransaction(const Transaction& tx) { pendingTransactions.push_back(tx); }


};


#endif //BLOCKCHAIN_HPP