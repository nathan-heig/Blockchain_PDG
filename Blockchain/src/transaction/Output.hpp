#ifndef OUTPUT_HPP
#define OUTPUT_HPP

#include "test/crypto.hpp"
#include <cereal/types/string.hpp>

/*Cette classe représente l'Output de transaction*/
class Output {
private:
    double value{};
    PubKey pubKey{};

public:
    Output() = default; // nécessaire pour la désérialisation
    Output(double value, const PubKey& pubKey)
        : value(value), pubKey(pubKey) {}

    const double getValue() const { return value; }
    const PubKey& getPubKey() const { return pubKey; }

    const std::string toString() const;


    template<class Archive>
    void serialize(Archive& ar){
        ar(value, pubKey);
    }
};


using Outputs = std::vector<Output>; // plus de const pour permettre la (dé)sérialisation

#endif // OUTPUT_HPP