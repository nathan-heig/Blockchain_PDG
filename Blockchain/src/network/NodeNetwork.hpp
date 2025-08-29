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

    void broadcastRaw(BinaryProtocol::MsgType type, const std::vector<uint8_t>& payload){
        auto frame = BinaryProtocol::buildMessage(type, payload);
        std::string s(reinterpret_cast<const char*>(frame.data()), frame.size());
        for (auto& [peer, c] : peers_) c->send(s);
        for (auto& c : incoming_) c->send(s);
    }

private:
    
    Blockchain& blockchain_;
    boost::asio::io_context io_;
    boost::asio::ip::tcp::acceptor acceptor_;
    uint16_t listenPort_;
    std::thread thread_;

    std::unordered_map<PeerInfo, ConnPtr> peers_; // connexions sortantes
    std::vector<ConnPtr> incoming_;

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

    void handleMessage(const PeerInfo& peer, const std::string& raw){
        // Un raw peut contenir exactement un frame (TcpConnection garantit framing length-only; ici header binaire custom) :
        if (raw.size() < sizeof(BinaryProtocol::MsgHeader)) return;
        std::vector<uint8_t> frame(raw.begin(), raw.end());
        BinaryProtocol::MsgHeader h; const uint8_t* payload = nullptr;
        try {
            BinaryProtocol::parseFrame(frame, h, payload);
        } catch(...) {
            return; // drop
        }
        auto type = static_cast<BinaryProtocol::MsgType>(h.type);
        switch(type){
            case BinaryProtocol::MsgType::VERSION: {
                // payload = struct Version { uint32_t height }
                if (h.length == sizeof(uint32_t)) {
                    uint32_t remoteSize; std::memcpy(&remoteSize, payload, sizeof(uint32_t));
                    sendVersion(peer); // réponse symétrique
                    sendInv(peer);
                    if (remoteSize > blockchain_.size())
                        requestBlocks(peer, blockchain_.size());
                }
                break;
            }
            case BinaryProtocol::MsgType::INV: {
                if (h.length == sizeof(uint32_t)) {
                    uint32_t best; std::memcpy(&best, payload, 4);
                    if (best > blockchain_.size())
                        requestBlocks(peer, blockchain_.size());
                }
                break;
            }
            case BinaryProtocol::MsgType::PING: {
                if (h.length == sizeof(uint64_t)) {
                    // renvoyer PONG même nonce
                    std::vector<uint8_t> pongPayload(payload, payload+sizeof(uint64_t));
                    sendFrame(peer, BinaryProtocol::MsgType::PONG, pongPayload);
                }
                break;
            }
            case BinaryProtocol::MsgType::PONG: {
                // mesurer latence si on stocke timestamp
                break;
            }
            case BinaryProtocol::MsgType::GET_BLOCKS: {
                // TODO: envoyer blocs manquants
                break;
            }
            default: break;
        }
    }

    void handleDisconnect(const PeerInfo& peer){
        peers_.erase(peer); // pour outgoing
        // incoming_ cleanup laissé simple (on garde l'objet jusqu'à GC vector)
    }

    void sendFrame(const PeerInfo& peer, BinaryProtocol::MsgType type, const std::vector<uint8_t>& payload){
        auto it = peers_.find(peer);
        if (it == peers_.end()) return;
        auto frame = BinaryProtocol::buildMessage(type, payload);
        std::string s(reinterpret_cast<const char*>(frame.data()), frame.size());
        it->second->send(s);
    }

    void sendVersion(const PeerInfo& peer){
        uint32_t height = blockchain_.size();
        auto payload = std::vector<uint8_t>(sizeof(uint32_t));
        std::memcpy(payload.data(), &height, 4);
        sendFrame(peer, BinaryProtocol::MsgType::VERSION, payload);
    }

    void sendInv(const PeerInfo& peer){
        uint32_t height = blockchain_.size();
        auto payload = std::vector<uint8_t>(sizeof(uint32_t));
        std::memcpy(payload.data(), &height, 4);
        sendFrame(peer, BinaryProtocol::MsgType::INV, payload);
    }

    void sendBlocks(const PeerInfo& peer, uint32_t lastIndex){
        for (uint32_t i = 0; i <= lastIndex; ++i) {
            const auto& block = blockchain_[i];

            auto payload = std::vector<uint8_t>(sizeof(Block));


            
            sendFrame(peer, BinaryProtocol::MsgType::BLOCK, payload);
        }
    }

    void requestBlocks(const PeerInfo& peer, uint32_t from){
        auto payload = std::vector<uint8_t>(sizeof(uint32_t));
        std::memcpy(payload.data(), &from, 4);
        sendFrame(peer, BinaryProtocol::MsgType::GET_BLOCKS, payload);
    }
};

#endif // NODE_NETWORK_HPP
