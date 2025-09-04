#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <string>
#include <vector>

using PubKey = std::string;
using Signature = std::string;
using Hash = std::string;


#include <stdexcept>
#include <vector>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <iomanip>
#include <string>


namespace crypto{

    EVP_PKEY* createPrivateKey();
    EVP_PKEY* getPrivateKey(const std::string& privateKeyFile);
    const PubKey getPubKey(const EVP_PKEY* pkey);
    void savePrivateKey(EVP_PKEY* pkey, const std::string& filename);
    
    Signature signData(const std::string& transaction, EVP_PKEY* pkey);
    bool verifySignature(const std::string& data, const Signature& signature, const PubKey& pubKey);

    Hash hashData(const std::string& data);

}
#endif // CRYPTO_HPP
