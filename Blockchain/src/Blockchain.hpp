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
    std::unordered_map<const PubKey, std::set<const OutputReference>> unspentOutputReferences;

    /*Ajoute une sortie non dépensée à la liste*/
    void addUnspentOutput(const PubKey& pubKey, const OutputReference& outputRef) {unspentOutputReferences[pubKey].insert(outputRef);}

public:
    // Constructor
    Blockchain();

    // Getters
    /*Cette fonction retourne les n premieres transactions*/
    const std::vector<const Transaction&>& getPendingTransactions(int number) const;
    /*Retourne le mining reward a un index donné*/
    const double getMiningRewardAt(uint32_t index) const{return 1000;}
    const std::unordered_map<const PubKey, std::set<const OutputReference>>& getUnspentOutputReferences() const {return unspentOutputReferences;}

    //Iterator
    /*Retourne un itérateur constant sur le début des blocs*/
    std::vector<const Block>::const_iterator cbegin() const {return blocks.cbegin();}
    /*Retourne un itérateur constant sur la fin des blocs*/
    std::vector<const Block>::const_iterator cend() const {return blocks.cend();}

    //Operators
    /*Retourne une référence constante sur le bloc à l'index donné*/
    const Block& operator[](const size_t index) const {return blocks[index];}

    //Setters
    /*Vérifie si le bloc est valide avant de l'ajouter à la blockchain et modifie la liste des sorties non dépensées*/
    void addBlock(const Block& block);
    /*Vérifie si la transaction est valide avant de l'ajouter à la pool*/
    void addPendingTransaction(const Transaction& tx);
};


#endif //BLOCKCHAIN_HPP