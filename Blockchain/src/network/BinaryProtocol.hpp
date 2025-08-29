#ifndef BINARY_PROTOCOL_HPP
#define BINARY_PROTOCOL_HPP

#include <cstdint>
#include <vector>
#include <cstring>
#include <stdexcept>
#include <type_traits>
#include <sstream>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>

/*Une frame contient une entete (MsgHeader) et un payload*/
using Frame = std::vector<uint8_t>;

// En-tête binaire fixe : magic(2) | version(1) | type(1) | length(4) | checksum(4)
// Total 12 octets (sans padding). length = taille du payload (hors header).
#pragma pack(push,1)
struct MsgHeader {
    uint16_t magic = 0xB17E;    // identifiant protocole (unifié)
    uint8_t  version = 1;       // version protocole
    uint8_t  type = 0;          // code message
    uint32_t length = 0;        // taille payload
    uint32_t checksum = 0;      // checksum pour vérifier l'intégrité
};
#pragma pack(pop)

static_assert(sizeof(MsgHeader) == 12, "MsgHeader size must be 12 bytes");

enum class MsgType : uint8_t {
    VERSION = 1,
    GET_BLOCK = 2,
    BLOCK = 3,
    GET_TXS = 4,
    TXS = 5,
    BROADCAST_TX = 6,
    BROADCAST_BLOCK = 7,
};



namespace BinaryProtocol {


    // Fonctions utilitaires (checksum simplifié)
    inline uint32_t simpleChecksum(const uint8_t* data, size_t len){
        uint32_t acc = 0; 
        for(size_t i=0;i<len;++i) 
            acc = (acc * 131) + data[i]; 
        return acc; 
    }



    // Encapsulation d'un message complet en binaire (header + payload)
    inline Frame buildFrame(MsgType type, const std::vector<uint8_t>& payload){
        MsgHeader h; 
        h.type = static_cast<uint8_t>(type); 
        h.length = static_cast<uint32_t>(payload.size());
        h.checksum = simpleChecksum(payload.data(), payload.size());

        std::vector<uint8_t> frame(sizeof(MsgHeader) + payload.size());
        std::memcpy(frame.data(), &h, sizeof(MsgHeader));
        if (!payload.empty())
            std::memcpy(frame.data()+sizeof(MsgHeader), payload.data(), payload.size());
        return frame;
    }

    /*Cette fonctione prend une frame et extrait l'en-tête et le pointeur vers le payload*/
    inline void parseFrame(const Frame& frame, MsgHeader& outHeader, const uint8_t*& payloadPtr){
        if (frame.size() < sizeof(MsgHeader))
            throw std::runtime_error("Frame too small");

        std::memcpy(&outHeader, frame.data(), sizeof(MsgHeader));
        if (outHeader.magic != 0xB17E)
            throw std::runtime_error("Bad magic");

        if (outHeader.version != 1)
            throw std::runtime_error("Bad version");

        if (frame.size() != sizeof(MsgHeader) + outHeader.length)
            throw std::runtime_error("Bad length");

        payloadPtr = frame.data() + sizeof(MsgHeader);
        uint32_t chk = simpleChecksum(payloadPtr, outHeader.length);
        if (chk != outHeader.checksum)
            throw std::runtime_error("Bad checksum");
    }

    // Sérialisation d'objets avec Cereal
    template<typename T, typename Archive = cereal::BinaryOutputArchive>
    std::vector<uint8_t> serializeObject(const T& obj){
        std::ostringstream oss(std::ios::binary);
        {
            Archive ar(oss);
            ar(obj);
        }
        auto str = oss.str();
        return std::vector<uint8_t>(str.begin(), str.end());
    }

    // Désérialisation d'objets avec Cereal
    template<typename T, typename Archive = cereal::BinaryInputArchive>
    T deserializeObject(const uint8_t* data, size_t len){
        std::istringstream iss(std::string(reinterpret_cast<const char*>(data), len), std::ios::binary);
        Archive ar(iss);
        T obj;
        ar(obj);
        return obj;
    }
}

#endif // BINARY_PROTOCOL_HPP
