#include "Blockchain.hpp"

const std::vector<const Transaction&>& Blockchain::getPendingTransactions(int number) const {

    std::vector<const Transaction&> vec;
    vec.reserve(number);
    auto it = pendingTransactions.begin();
    for (int i = 0; i < number && it != pendingTransactions.end(); ++i, ++it) {
        vec.push_back(*it);
    }
    return vec;
} 
