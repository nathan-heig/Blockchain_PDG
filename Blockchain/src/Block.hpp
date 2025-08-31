#ifndef BLOCK_HPP
#define BLOCK_HPP


#include "transaction/BlockTransactions.hpp"
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>
#include <sstream>

class Blockchain;

struct Target{
    uint8_t value;
    uint8_t max;

    static Target createInitialTarget() {
        return Target{3, 128};
    }
    inline static const uint8_t sizePerStep = 8;

    void augmenterDifficulte(int step) {
        if (max <= step * sizePerStep) {
            if (value < 32) {
                value++;
                max = 255 - ((step * sizePerStep) - max);
            }
        } else {
            max -= step * sizePerStep;
        }
    }
    void diminuerDifficulte(int step) {
        if (max + step * sizePerStep >= 256) {
            if (value > 0) {
                value--;
                max = (max + step * sizePerStep) - 256;
            }
        } else {
            max += step * sizePerStep;
        }
    }

    template<class Archive>
    void serialize(Archive& ar) {
        ar(value, max);
    }
};


class Block {
private:
    // Attributs
    uint32_t index;
    uint32_t nonce;

    uint32_t timestamp;
    Target target;

    BlockTransactions transactions;

    Hash previousHash;
    Hash hash;

    template<typename Archive>
    void serialize_without_hash(Archive& ar) const {
        ar(index, nonce, timestamp, target, transactions, previousHash);
    }

    // Fonctions
    Hash calculateHash() const {
        std::ostringstream oss(std::ios::binary);
        {
            cereal::BinaryOutputArchive ar(oss);
            serialize_without_hash(ar); // sérialise tous les champs sauf 'hash'
        }
        return crypto::hashData(oss.str());
    }
    const bool hashMatchesDifficulty() const;

public:

    Block() = default; // pour désérialisation

    /*Crée un nouveau bloc à partir de la blockchain et de la clé publique du mineur*/
    static Block createBlock(const Blockchain& blockchain, const PubKey& minerPubKey);

    // Operators
    const Transaction& operator[](const size_t i) const {return transactions[i];}


    // Getters
    const uint32_t getIndex() const { return index; }
    const uint32_t getTimestamp() const { return timestamp; }
    const Hash& getPreviousHash() const { return previousHash; }
    const Hash& getHash() const { return hash; }
    const BlockTransactions& getBlockTransactions() const {return transactions;}
    const Target& getTarget() const { return target; }

    /*Cette fonction vérifie la validité du bloc en s'assurant que le hash correspond à la difficulté et que les transactions sont valides.*/
    bool verify(const Blockchain& blockchain, const UTXOs& utxos) const;

    template<class Archive>
    void serialize(Archive& ar){
        ar(index, nonce, timestamp, target, transactions, previousHash, hash);
    }

};

#endif //BLOCKCHAIN_CLASS_HPP