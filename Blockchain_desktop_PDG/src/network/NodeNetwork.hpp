#ifndef NODE_NETWORK_HPP
#define NODE_NETWORK_HPP

#include <boost/asio.hpp>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <chrono>
#include <semaphore>
#include <random>
#include "TcpConnection.hpp"
#include "PeerInfo.hpp"
#include "BinaryProtocol.hpp"
#include "config.hpp"
#include <atomic>

#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#include <miniupnpc/upnperrors.h>

class Blockchain;

// Exemple complet d'un mini gestionnaire de réseau avec handshake + ping
class NodeNetwork {
public:
    using ConnPtr = std::shared_ptr<TcpConnection>;

    explicit NodeNetwork(Blockchain& blockchain)
        : io_(), acceptor_(io_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), LISTEN_PORT)), listenPort_(LISTEN_PORT), blockchain_(blockchain) {}

    /*Lance un thread qui gère le réseau*/
    void start() {
        (void)openPort();
        accept();
        thread_ = std::thread([this]{ io_.run(); });
    }
    void stop() {
        io_.stop();
        if (thread_.joinable()) thread_.join();
        closePort();
    }

    void connect(const PeerInfo& peer) {
        auto conn = std::make_shared<TcpConnection>(io_);
        conn->socketRef().async_connect({ boost::asio::ip::make_address(peer.getIp()), peer.getPort() },
            [this, conn, peer](auto ec){
                if (!ec){
                    peers_[peer] = conn;
                    peerCount_.fetch_add(1, std::memory_order_relaxed);
                    setupCallbacks(peer, conn);
                    sendVersion(peer);
                }
            });
    }

    int peerCount() const { return peerCount_.load(); }

    void buildFrameAndbroadcast(MsgType type, const std::vector<uint8_t>& payload);

    template<typename T>
    void buildFrameAndbroadcast(MsgType type, const T& obj) {
        auto payload = BinaryProtocol::serializeObject(obj);
        buildFrameAndbroadcast(type, payload);
    }

private:

    Blockchain& blockchain_;
    boost::asio::io_context io_;
    boost::asio::ip::tcp::acceptor acceptor_;
    uint16_t listenPort_;
    std::thread thread_;

    std::unordered_map<PeerInfo, ConnPtr> peers_; // connexions sortantes
    std::unordered_map<PeerInfo, ConnPtr> incoming_; // connexions entrantes
    std::unordered_map<PeerInfo, uint32_t> peersSize_;

    std::atomic<int> peerCount_{0};

    void accept(){
        auto conn = std::make_shared<TcpConnection>(io_);
        acceptor_.async_accept(conn->socketRef(), [this, conn](auto ec){
            if (!ec){
                PeerInfo p(conn->socketRef().remote_endpoint().address().to_string(), conn->socketRef().remote_endpoint().port());
                incoming_[p] = conn;
                peerCount_.fetch_add(1, std::memory_order_relaxed);
                setupCallbacks(p, conn);
                std::cout << "New incoming connection from: " << p.getIp() << ":" << p.getPort() << std::endl;
            }
            else{
                std::cerr << "Accept error: " << ec.message() << std::endl;
            }
            accept();
        });
    }

    void setupCallbacks(const PeerInfo& peer, const ConnPtr& conn){
        auto self = this;
        conn->start(
            [this, peer](const std::string& msg){ handleMessage(peer, msg); },
            [this, peer]{ handleDisconnect(peer); }
        );
    }

    void broadcastBack(const std::string& raw, const PeerInfo& exclude);

    void handleMessage(const PeerInfo& peer, const std::string& raw);
    void handleDisconnect(const PeerInfo& peer);

    void buildAndSendFrame(const PeerInfo& peer, MsgType type, const std::vector<uint8_t>& payload);

    void sendVersion(const PeerInfo& peer);

    void sendAck(const PeerInfo& peer);

    void sendBlock(const PeerInfo& peer, uint32_t blockIdx);

    void requestBlock(const PeerInfo& peer, uint32_t blockIdx);

    bool openPort() {
        bool success = false;

        UPNPDev* devlist = nullptr;
        int error = 0;

        // upnpDiscover signature "récente" (avec TTL). OK avec vcpkg.
        devlist = upnpDiscover(2000, /*multicastif*/nullptr, /*minissdpdsock*/nullptr,
                               /*sameport*/0, /*ipv6*/0, /*ttl*/2, &error);

        UPNPUrls urls{};
        IGDdatas data{};
        char lanaddr[64] = {0};
        char wanaddr[64] = {0};

        // Compat API : versions récentes demandent WAN aussi.
        int igd = 0;
#if defined(MINIUPNPC_API_VERSION) && (MINIUPNPC_API_VERSION >= 18)
        igd = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr),
                               wanaddr, sizeof(wanaddr));
#else
        igd = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr));
#endif

        freeUPNPDevlist(devlist);

        if (igd == 1) {
            std::string port_str = std::to_string(listenPort_);
            int rc = UPNP_AddPortMapping(urls.controlURL, data.first.servicetype,
                                         port_str.c_str(), port_str.c_str(),
    #if defined(MINIUPNPC_API_VERSION) && (MINIUPNPC_API_VERSION >= 18)
                                         lanaddr,
    #else
                                         lanaddr, // ancienne API, même paramètre
    #endif
                                         "Blockchain", "TCP", nullptr, "0");
            success = (rc == UPNPCOMMAND_SUCCESS);
            FreeUPNPUrls(&urls);
        } else {
            // Pas d'IGD valide → on continue sans UPnP
            success = false;
        }

        if (success)
            std::cout << "UPnP mapping OK on port " << listenPort_ << std::endl;
        else
            std::cout << "UPnP mapping not available; continuing local listen" << std::endl;

        return success;
    }

    void closePort() {
        UPNPDev* devlist = nullptr;
        int error = 0;

        devlist = upnpDiscover(2000, nullptr, nullptr, 0, 0, 2, &error);

        UPNPUrls urls{};
        IGDdatas data{};
        char lanaddr[64] = {0};
        char wanaddr[64] = {0};

        int igd = 0;
#if defined(MINIUPNPC_API_VERSION) && (MINIUPNPC_API_VERSION >= 18)
        igd = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr),
                               wanaddr, sizeof(wanaddr));
#else
        igd = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr));
#endif

        freeUPNPDevlist(devlist);

        if (igd == 1) {
            std::string port_str = std::to_string(listenPort_);
            int rc = UPNP_DeletePortMapping(urls.controlURL, data.first.servicetype,
                                            port_str.c_str(), "TCP", nullptr);
            if (rc != UPNPCOMMAND_SUCCESS) {
                std::cout << "Failed to delete port mapping" << std::endl;
            }
            FreeUPNPUrls(&urls);
        }
    }

};

#endif // NODE_NETWORK_HPP