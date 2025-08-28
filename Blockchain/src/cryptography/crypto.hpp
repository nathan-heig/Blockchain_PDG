#ifndef KEY_HPP
#define KEY_HPP

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
#include <iomanip>
#include <string>


namespace crpto{

    EVP_PKEY* createPrivateKey();
    EVP_PKEY* getPrivateKey(const std::string& privateKeyFile);
    const PubKey getPubKey(const EVP_PKEY* pkey);
    void savePrivateKey(EVP_PKEY* pkey, const std::string& filename);
    
    Signature signData(const std::string& transaction, EVP_PKEY* pkey);
    bool verifySignature(const std::string& data, const Signature& signature, const PubKey& pubKey);

    Hash hashData(const std::string& data) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), hash);
        return Hash(hash, hash + SHA256_DIGEST_LENGTH);
    }

}
#endif // KEY_HPP
