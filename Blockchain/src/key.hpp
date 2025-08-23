#ifndef KEY_HPP
#define KEY_HPP

#include <string>
#include <vector>

using PubKey = std::string;
using Signature = std::string;
using Signatures = std::vector<Signature>;


#include <stdexcept>
#include <vector>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <iomanip>
#include <string>


namespace key{

    EVP_PKEY* createPrivateKey();
    const Signature signData(const std::string& transaction, const std::string& privateKeyFile);
    const PubKey getPubKey(const EVP_PKEY* pkey);
    void savePrivateKey(EVP_PKEY* pkey, const std::string& filename);
    bool verifySignature(const std::string& data, const std::string& signature, const std::string& pubKey);

}
#endif // KEY_HPP
