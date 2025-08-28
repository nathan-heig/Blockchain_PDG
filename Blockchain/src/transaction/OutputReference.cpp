#include "OutputReference.hpp"
#include "Blockchain.hpp"

const Output& OutputReference::getOutput(const Blockchain& blockchain) const {
    return blockchain[blockIndex][txIndex].getOutputs()[outputIndex];
}