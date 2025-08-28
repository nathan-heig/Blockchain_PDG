#ifndef OUTPUTREFERENCE_HPP
#define OUTPUTREFERENCE_HPP

#include <cstdint>
#include <sstream>
#include <tuple>
#include "Output.hpp"

class Blockchain;


/*Cette classe représente une référence à une sortie de transaction utilisée comme Input d'une autre transaction*/
class OutputReference {
private:
    uint32_t blockIndex;
    uint16_t txIndex;
    uint16_t outputIndex;

public:
    //Constructor
    OutputReference(uint32_t blockIdx, uint16_t txIdx, uint16_t outIdx)
        : blockIndex(blockIdx), txIndex(txIdx), outputIndex(outIdx) {}
    

    //Comparison operator
    bool operator<(const OutputReference& other) const {
        return std::tie(blockIndex, txIndex, outputIndex) < std::tie(other.blockIndex, other.txIndex, other.outputIndex);
    }


    //Getters
    const Output& getOutput(const Blockchain& blockchain) const;

    //String representation
    const std::string toString() const {
        std::ostringstream oss;
        oss << "Block: " << blockIndex << ", Tx: " << txIndex << ", Out: " << outputIndex;
        return oss.str();
    }

};
using Inputs = std::vector<const OutputReference>;



#endif // OUTPUTREFERENCE_HPP