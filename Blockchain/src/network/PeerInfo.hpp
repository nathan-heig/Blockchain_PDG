// PeerInfo.hpp
#ifndef PEER_INFO_HPP
#define PEER_INFO_HPP

#include <string>
#include <cstdint>
#include <tuple>
#include <functional>

// Représente l'identité d'un pair (sans gérer la connexion réseau elle-même)
class PeerInfo {
private:
    std::string ip;
    uint16_t port;

public:
    PeerInfo() = default;
    PeerInfo(const std::string& ip_, uint16_t port_)
        : ip(ip_), port(port_) {}

    const std::string& getIp() const { return ip; }
    uint16_t getPort() const { return port; }

    bool operator<(const PeerInfo& other) const {
        return std::tie(ip, port) < std::tie(other.ip, other.port);
    }
    bool operator==(const PeerInfo& other) const {
        return ip == other.ip && port == other.port;
    }
    bool operator!=(const PeerInfo& other) const {
        return !(*this == other);
    }

    std::string toString() const {
        return ip + ":" + std::to_string(port);
    }

};

// Spécialisation de std::hash pour permettre l'utilisation de PeerInfo
// comme clé dans std::unordered_map / std::unordered_set.
namespace std {
    template<> struct hash<PeerInfo> {
        size_t operator()(const PeerInfo& p) const noexcept {
            return hash<string>()(p.toString());
        }
    };
}

#endif // PEER_INFO_HPP