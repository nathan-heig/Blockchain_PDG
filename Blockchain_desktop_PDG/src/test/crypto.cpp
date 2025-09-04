#include "test/crypto.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>

namespace crypto {


    EVP_PKEY* createPrivateKey() {

        void seedOpenSSL() {
            if (!RAND_status()) {
                // Seed from various entropy sources
                if (RAND_poll() == 0) {
                    throw std::runtime_error("Failed to seed OpenSSL RNG");
                }
            }
        }


        EVP_PKEY* pkey = nullptr;
        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);
        if (!ctx) {
            throw std::runtime_error("Failed to create EVP_PKEY_CTX");
        }
        if (EVP_PKEY_keygen_init(ctx) <= 0) {
            EVP_PKEY_CTX_free(ctx);
            throw std::runtime_error("Failed to initialize keygen context");
        }
        // Utilise la courbe secp256k1 (utilisée par Bitcoin)
        if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, NID_secp256k1) <= 0) {
            EVP_PKEY_CTX_free(ctx);
            throw std::runtime_error("Failed to set EC curve");
        }
        if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
            EVP_PKEY_CTX_free(ctx);
            throw std::runtime_error("Failed to generate EC key");
        }
        EVP_PKEY_CTX_free(ctx);
        return pkey;
    }

    void savePrivateKey(EVP_PKEY* pkey, const std::string& filename) {
        BIO* bio = BIO_new_file(filename.c_str(), "w");
        if (!bio) {
            throw std::runtime_error("Failed to open file");
        }

        if (PEM_write_bio_PrivateKey(bio, pkey, NULL, NULL, 0, NULL, NULL) != 1) {
            BIO_free(bio);
            throw std::runtime_error("Failed to write private key");
        }

        BIO_free(bio);
    }

    EVP_PKEY* getPrivateKey(const std::string& privateKeyFile) {
        BIO* bio = BIO_new_file(privateKeyFile.c_str(), "r");
        if (!bio) {
            throw std::runtime_error("Failed to open private key file");
        }

        EVP_PKEY* pkey = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
        BIO_free(bio);
        if (!pkey) {
            throw std::runtime_error("Failed to read private key");
        }

        return pkey;
    }

    Signature signData(const std::string& data, EVP_PKEY* pkey) {
        if (!pkey) {
            throw std::runtime_error("Invalid private key");
        }
        // Create signature context
        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        if (!ctx) {
            throw std::runtime_error("Failed to create EVP_MD_CTX");
        }
        // Initialize signing context
        if (EVP_DigestSignInit(ctx, NULL, EVP_sha256(), NULL, pkey) != 1) {
            EVP_MD_CTX_free(ctx);
            throw std::runtime_error("Failed to initialize signing context");
        }
        // Add data to sign
        if (EVP_DigestSignUpdate(ctx, data.c_str(), data.size()) != 1) {
            EVP_MD_CTX_free(ctx);
            throw std::runtime_error("Failed to add data to sign");
        }
        // Get required buffer size
        size_t signatureLen;
        if (EVP_DigestSignFinal(ctx, NULL, &signatureLen) != 1) {
            EVP_MD_CTX_free(ctx);
            EVP_PKEY_free(pkey);
            throw std::runtime_error("Failed to get required buffer size");
        }

        // Create buffer for signature
        std::vector<unsigned char> signature(signatureLen);

        // Generate signature
        if (EVP_DigestSignFinal(ctx, signature.data(), &signatureLen) != 1) {
            EVP_MD_CTX_free(ctx);
            EVP_PKEY_free(pkey);
            throw std::runtime_error("Failed to generate signature");
        }

        // Cleanup
        EVP_MD_CTX_free(ctx);

        // Resize vector to actual size of signature
        signature.resize(signatureLen);

        std::string signatureStr(signature.begin(), signature.end());

        return signatureStr;
    }

    const PubKey getPubKey(const EVP_PKEY* pkey) {
        if (!pkey) {
            throw std::runtime_error("Invalid EVP_PKEY");
        }

        // Get public key from private key
        BIO* bio = BIO_new(BIO_s_mem());
        if (!bio) {
            throw std::runtime_error("Failed to create BIO");
        }

        if (PEM_write_bio_PUBKEY(bio, pkey) != 1) {
            BIO_free(bio);
            throw std::runtime_error("Failed to write public key");
        }

        // Read public key from BIO
        char* pubKey;
        size_t pubKeyLen = BIO_get_mem_data(bio, &pubKey);
        std::string pubKeyStr(pubKey, pubKeyLen);

        // Remove PEM headers and footers
        std::string beginMarker = "-----BEGIN PUBLIC KEY-----";
        std::string endMarker = "-----END PUBLIC KEY-----";
        
        size_t start = pubKeyStr.find(beginMarker);
        if (start != std::string::npos) {
            start += beginMarker.length();
            size_t end = pubKeyStr.find(endMarker);
            if (end != std::string::npos) {
                pubKeyStr = pubKeyStr.substr(start, end - start);
                // Remove newlines and whitespace
                pubKeyStr.erase(std::remove_if(pubKeyStr.begin(), pubKeyStr.end(), 
                    [](char c) { return std::isspace(c); }), pubKeyStr.end());
            }
        }

        // Cleanup
        BIO_free(bio);

        return pubKeyStr;
    }

    bool verifySignature(const std::string& data, const Signature& signature, const PubKey& pubKey) {

        std::string keyPem = "-----BEGIN PUBLIC KEY-----\n" + pubKey + "\n-----END PUBLIC KEY-----";

        // Load public key
        BIO* bio = BIO_new_mem_buf(keyPem.data(), keyPem.size());
        EVP_PKEY* pkey = PEM_read_bio_PUBKEY(bio, NULL, NULL, NULL);
        BIO_free(bio);
        if (!pkey) {
            return false;
        }

        // Create verification context
        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        if (!ctx) {
            EVP_PKEY_free(pkey);
            return false;
        }

        // Initialize verification context
        if (EVP_DigestVerifyInit(ctx, NULL, EVP_sha256(), NULL, pkey) != 1) {
            EVP_MD_CTX_free(ctx);
            EVP_PKEY_free(pkey);
            return false;
        }

        if (EVP_DigestVerifyUpdate(ctx, data.c_str(), data.size()) != 1) {
            EVP_MD_CTX_free(ctx);
            EVP_PKEY_free(pkey);
            return false;
        }

        // Verify signature
        bool isValid = EVP_DigestVerifyFinal(ctx, reinterpret_cast<const unsigned char*>(signature.data()), signature.size()) == 1;

        // Cleanup
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(pkey);

        return isValid;
    }

    Hash hashData(const std::string& data) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), hash);
        return Hash(hash, hash + SHA256_DIGEST_LENGTH);
    }

}