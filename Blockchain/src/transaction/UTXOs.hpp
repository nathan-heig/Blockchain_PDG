#ifndef UTXOS_HPP
#define UTXOS_HPP

#include "OutputReference.hpp"

#include <unordered_map>
#include <set>

using UTXOs = std::unordered_map<PubKey, std::set<OutputReference>>;

#endif //UTXOS_HPP