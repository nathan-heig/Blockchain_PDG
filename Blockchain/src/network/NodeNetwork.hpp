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
        if (openPort()) {
            accept();
        }
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
                    setupCallbacks(peer, conn);
                    sendVersion(peer);
                }
            });
    }

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

    

    void accept(){
        auto conn = std::make_shared<TcpConnection>(io_);
        acceptor_.async_accept(conn->socketRef(), [this, conn](auto ec){
            if (!ec){
                PeerInfo p(conn->socketRef().remote_endpoint().address().to_string(), conn->socketRef().remote_endpoint().port());
                incoming_[p] = conn;
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
        bool success = true;

        UPNPDev* devlist = nullptr;
        std::string lanaddr(64, '\0');

        int error = 0;
        devlist = upnpDiscover(2000, nullptr, nullptr, 0, 0, 2, &error);

        UPNPUrls urls;
        IGDdatas data;
        error = UPNP_GetValidIGD(devlist, &urls, &data, &lanaddr[0], lanaddr.size());
        freeUPNPDevlist(devlist);

        if (error != 1) {
            //std::cout << "Failed to get IGD" << std::endl;
            success = false;
        }

        std::string port_str = std::to_string(listenPort_);
        error = UPNP_AddPortMapping(urls.controlURL, data.first.servicetype,
                                    port_str.c_str(), port_str.c_str(), lanaddr.c_str(), "Blockchain", "TCP", nullptr, "0");

        if (error != UPNPCOMMAND_SUCCESS) {
            //std::cout << "Failed to add port mapping" << std::endl;
            success = false;
        }

        FreeUPNPUrls(&urls);

        if (success) {
            std::cout << "Successfully opened port, listening on " << listenPort_ << std::endl;
        } else {
            std::cout << "Failed to open port" << std::endl;
            listenPort_ = 0;
        }

        return success;
    }
    void closePort() {
        UPNPDev* devlist = nullptr;
        std::string lanaddr(64, '\0');

        int error = 0;
        devlist = upnpDiscover(2000, nullptr, nullptr, 0, 0, 2, &error);

        UPNPUrls urls;
        IGDdatas data;
        error = UPNP_GetValidIGD(devlist, &urls, &data, &lanaddr[0], lanaddr.size());
        freeUPNPDevlist(devlist);

        if (error != 1) {
            std::cout << "Failed to get IGD" << std::endl;
        }

        std::string port_str = std::to_string(listenPort_);
        error = UPNP_DeletePortMapping(urls.controlURL, data.first.servicetype, port_str.c_str(), "TCP", nullptr);

        if (error != UPNPCOMMAND_SUCCESS) {
            std::cout << "Failed to delete port mapping" << std::endl;
        }

        FreeUPNPUrls(&urls);
    }
};

#endif // NODE_NETWORK_HPP
