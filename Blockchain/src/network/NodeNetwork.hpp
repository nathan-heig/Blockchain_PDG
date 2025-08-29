#ifndef NODE_NETWORK_HPP
#define NODE_NETWORK_HPP

#include <boost/asio.hpp>
#include <thread>
#include <unordered_map>
#include <random>
#include "TcpConnection.hpp"
#include "PeerInfo.hpp"
#include "BinaryProtocol.hpp"
#include "config.hpp"
#include "Blockchain.hpp"


// Exemple complet d'un mini gestionnaire de réseau avec handshake + ping
class NodeNetwork {
public:
    using ConnPtr = std::shared_ptr<TcpConnection>;

    explicit NodeNetwork(Blockchain& blockchain)
        : io_(), acceptor_(io_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), LISTEN_PORT)), listenPort_(LISTEN_PORT), blockchain_(blockchain) {}

    void start() {
        accept();
        thread_ = std::thread([this]{ io_.run(); });
    }
    void stop() {
        io_.stop();
        if (thread_.joinable()) thread_.join();
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

    void buildAndbroadcastFrame(MsgType type, const std::vector<uint8_t>& payload){
        auto frame = BinaryProtocol::buildFrame(type, payload);
        std::string s(reinterpret_cast<const char*>(frame.data()), frame.size());
        for (auto& [peer, c] : peers_)
            c->send(s);
        for (auto& [peer, c] : incoming_)
            c->send(s);
    }

private:
    
    Blockchain& blockchain_;
    boost::asio::io_context io_;
    boost::asio::ip::tcp::acceptor acceptor_;
    uint16_t listenPort_;
    std::thread thread_;

    std::unordered_map<PeerInfo, ConnPtr> peers_; // connexions sortantes
    std::unordered_map<PeerInfo, ConnPtr> incoming_; // connexions entrantes

    void accept(){
        auto conn = std::make_shared<TcpConnection>(io_);
        acceptor_.async_accept(conn->socketRef(), [this, conn](auto ec){
            if (!ec){
                PeerInfo p(conn->socketRef().remote_endpoint().address().to_string(), conn->socketRef().remote_endpoint().port());
                incoming_.push_back(conn);
                setupCallbacks(p, conn);
                // On attend sa Version -> on ne répond qu'après réception
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

    void broadcastBack(const std::string& raw, const PeerInfo& exclude){
        std::string s = raw;
        for (auto& [peer, c] : peers_)
            if (peer != exclude)
                c->send(s);
        for (auto& [peer, c] : incoming_)
            if (peer != exclude)
                c->send(s);
    }

    void handleMessage(const PeerInfo& peer, const std::string& raw){
        // Un raw peut contenir exactement un frame (TcpConnection garantit framing length-only; ici header binaire custom) :
        if (raw.size() < sizeof(MsgHeader))
            return;

        Frame frame(raw.begin(), raw.end());
        MsgHeader h;
        const uint8_t* payload = nullptr;
        try {
            BinaryProtocol::parseFrame(frame, h, payload);
        } catch(...) {
            return; // drop
        }

        auto type = static_cast<MsgType>(h.type);
        switch(type){
            case MsgType::VERSION: {
                if (h.length == sizeof(uint32_t)) {
                    uint32_t remoteSize; std::memcpy(&remoteSize, payload, sizeof(uint32_t));
                    if (remoteSize > blockchain_.size())
                        requestBlock(peer, blockchain_.size());
                }
                break;
            }
            case MsgType::GET_BLOCK: {
                if (h.length == sizeof(uint32_t)) {
                    uint32_t blockIdx; std::memcpy(&blockIdx, payload, sizeof(uint32_t));
                    sendBlock(peer, blockIdx);
                }
                break;
            }
            case MsgType::BLOCK: {
                try {
                    Block b = BinaryProtocol::deserializeObject<Block>(payload, h.length);

                    // TODO: validation hors thread réseau
                   blockchain_.addBlock(b);
                } catch(...) {}
                break;
            }
            case MsgType::BROADCAST_TX: {

                try {
                    Transaction tx = BinaryProtocol::deserializeObject<Transaction>(payload, h.length);
                    // TODO: validation hors thread réseau
                    if (blockchain_.addPendingTransaction(tx)) {
                        broadcastBack(raw, peer);
                    }
                } catch(...) {}
                break;
            }
            case MsgType::BROADCAST_BLOCK: {

                try {
                    Block b = BinaryProtocol::deserializeObject<Block>(payload, h.length);
                    // TODO: validation hors thread réseau
                    if (blockchain_.addBlock(b)) {
                        broadcastBack(raw, peer);
                    }
                } catch(...) {}
                break;
            }
            default: break;
        }
    }

    void handleDisconnect(const PeerInfo& peer){
        peers_.erase(peer); // pour outgoing
    }

    void buildAndSendFrame(const PeerInfo& peer, MsgType type, const std::vector<uint8_t>& payload){
        auto it = peers_.find(peer);
        if (it == peers_.end())
            return; //si le peer n'est pas trouvé
        auto frame = BinaryProtocol::buildFrame(type, payload);
        std::string s(reinterpret_cast<const char*>(frame.data()), frame.size());
        it->second->send(s);
    }

    void sendVersion(const PeerInfo& peer){
        uint32_t height = blockchain_.size();
        auto payload = std::vector<uint8_t>(sizeof(height));
        std::memcpy(payload.data(), &height, sizeof(height));
        buildAndSendFrame(peer, MsgType::VERSION, payload);
    }

    void sendBlock(const PeerInfo& peer, uint32_t blockIdx){
        const auto& block = blockchain_[blockIdx];
        auto payload = BinaryProtocol::serializeObject<Block>(block);
        buildAndSendFrame(peer, MsgType::BLOCK, payload);
        
    }

    void requestBlock(const PeerInfo& peer, uint32_t blockIdx){
        auto payload = std::vector<uint8_t>(sizeof(blockIdx));
        std::memcpy(payload.data(), &blockIdx, sizeof(blockIdx));
        buildAndSendFrame(peer, MsgType::GET_BLOCK, payload);
    }
};

#endif // NODE_NETWORK_HPP
