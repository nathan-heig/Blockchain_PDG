#ifndef OUTPUT_HPP
#define OUTPUT_HPP

#include "cryptography/crypto.hpp"

/*Cette classe représente l'Output de transaction*/
class Output {
private:
    double value;
    PubKey pubKey;

public:
    Output(double value, const PubKey& pubKey)
        : value(value), pubKey(pubKey) {}

    const double getValue() const { return value; }
    const PubKey& getPubKey() const { return pubKey; }

    const std::string toString() const;
};


using Outputs = std::vector<const Output>;

#endif // OUTPUT_HPP