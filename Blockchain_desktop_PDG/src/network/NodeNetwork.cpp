#include "NodeNetwork.hpp"
#include "Blockchain.hpp"
#include <iostream>
#include <chrono>


void NodeNetwork::broadcastBack(const std::string& raw, const PeerInfo& exclude){
    std::string s = raw;
    for (auto& [peer, c] : peers_)
        if (peer != exclude)
            c->send(s);
    for (auto& [peer, c] : incoming_)
        if (peer != exclude)
            c->send(s);
}

void NodeNetwork::handleMessage(const PeerInfo& peer, const std::string& raw){
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
            if (h.length == sizeof(uint16_t)) {
                std::cout << "Received VERSION message" << std::endl;
                uint16_t remoteListeningPort;
                std::memcpy(&remoteListeningPort, payload, sizeof(uint16_t));


                if (h.localSize > blockchain_.size())
                    requestBlock(peer, blockchain_.size());
                else{
                    synchronized_ = true;
                }

                sendAck(peer);
            }
            break;
        }
        case MsgType::ACK: {
            if (h.length == sizeof(uint16_t)) {
                std::cout << "Received ACK message" << std::endl;
                uint16_t remoteListeningPort;
                std::memcpy(&remoteListeningPort, payload, sizeof(uint16_t));

                if (h.localSize > blockchain_.size())
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


                if(blockchain_.addBlock(b)) {
                    if(h.localSize > blockchain_.size()){
                        requestBlock(peer, blockchain_.size());
                    }else{
                        synchronized_ = true;
                    }
                }
            } catch(...) {}
            break;
        }
        case MsgType::BROADCAST_TX: {

            try {
                Transaction tx = BinaryProtocol::deserializeObject<Transaction>(payload, h.length);

                if (blockchain_.getTransactionPool().addTransaction(tx)) {
                    broadcastBack(raw, peer);
                }
            } catch(...) {}
            break;
        }
        case MsgType::BROADCAST_BLOCK: {

            try {
                Block b = BinaryProtocol::deserializeObject<Block>(payload, h.length);

                if (blockchain_.addBlock(b)) {
                    broadcastBack(raw, peer);
                } else if (h.localSize > blockchain_.size() + 1) {
                    requestBlock(peer, blockchain_.size());
                }
                
            } catch(...) {}
            break;
        }
        default: break;
    }
}

void NodeNetwork::handleDisconnect(const PeerInfo& peer){
    size_t erased = peers_.erase(peer); // pour outgoing
    if (!erased) erased = incoming_.erase(peer); // pour incoming
    if (erased) peerCount_.fetch_sub(1, std::memory_order_relaxed);
}

void NodeNetwork::buildAndSendFrame(const PeerInfo& peer, MsgType type, const std::vector<uint8_t>& payload){
    auto it = peers_.find(peer);
    if (it == peers_.end()){
        it = incoming_.find(peer);
        if (it == incoming_.end())
            return; // si le peer n'est pas trouvé
    }

    auto frame = BinaryProtocol::buildFrame(type, blockchain_.size(), payload);
    std::string s(reinterpret_cast<const char*>(frame.data()), frame.size());
    it->second->send(s);
}

void NodeNetwork::sendVersion(const PeerInfo& peer){
    uint16_t listeningPort = listenPort_;
    auto payload = std::vector<uint8_t>(sizeof(listeningPort));
    std::memcpy(payload.data(), &listeningPort, sizeof(listeningPort));
    buildAndSendFrame(peer, MsgType::VERSION, payload);
}

void NodeNetwork::sendAck(const PeerInfo& peer){
    uint16_t listeningPort = listenPort_;
    auto payload = std::vector<uint8_t>(sizeof(listeningPort));
    std::memcpy(payload.data(), &listeningPort, sizeof(listeningPort));
    buildAndSendFrame(peer, MsgType::ACK, payload);
}

void NodeNetwork::sendBlock(const PeerInfo& peer, uint32_t blockIdx){
    const auto& block = blockchain_[blockIdx];
    auto payload = BinaryProtocol::serializeObject<Block>(block);
    buildAndSendFrame(peer, MsgType::BLOCK, payload);
}

void NodeNetwork::requestBlock(const PeerInfo& peer, uint32_t blockIdx){
    std::cout << "Requesting block " << blockIdx << " from peer " << peer.getIp() << std::endl;

    auto payload = std::vector<uint8_t>(sizeof(blockIdx));
    std::memcpy(payload.data(), &blockIdx, sizeof(blockIdx));
    buildAndSendFrame(peer, MsgType::GET_BLOCK, payload);
}


void NodeNetwork::buildFrameAndbroadcast(MsgType type, const std::vector<uint8_t>& payload){
    auto frame = BinaryProtocol::buildFrame(type, blockchain_.size(), payload);
    std::string s(reinterpret_cast<const char*>(frame.data()), frame.size());
    for (auto& [peer, c] : peers_)
        c->send(s);
    for (auto& [peer, c] : incoming_)
        c->send(s);
}