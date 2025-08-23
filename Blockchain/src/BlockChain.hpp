#ifndef BLOCKCHAIN_HPP
#define BLOCKCHAIN_HPP

#include "Block.hpp"

#include <unordered_map>
#include <set>

class Blockchain {
private:
    std::vector<const Block> blocks;
    std::set<const Transaction, Transaction::CompareByFees> pendingTransactions;
    std::unordered_map<PubKey, std::set<OutputReference>> UTXOs;

public:
    // Constructor
    Blockchain() {}

    // Getters
    const std::vector<const Transaction&>& getPendingTransactions(int number) const {
        std::vector<const Transaction&> vec;
        vec.reserve(number);
        auto it = pendingTransactions.begin();
        for (int i = 0; i < number && it != pendingTransactions.end(); ++i, ++it) {
            vec.push_back(*it);
        }
        return vec;
    }       

    //iterator
    std::vector<const Block>::const_iterator cbegin() const {return blocks.cbegin();}
    std::vector<const Block>::const_iterator cend() const {return blocks.cend();}

    //operators
    const Block& operator[](size_t index) const {return blocks[index];}


};


#endif //BLOCKCHAIN_HPP