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
            if (h.length == sizeof(uint32_t) + sizeof(uint16_t)) {
                sendAck(peer);//accepte la connection
                uint32_t remoteSize;
                std::memcpy(&remoteSize, payload, sizeof(uint32_t));
                uint16_t remoteListeningPort;
                std::memcpy(&remoteListeningPort, payload + sizeof(uint32_t), sizeof(uint16_t));
                if (remoteSize > blockchain_.size())
                    requestBlocks(peer, remoteSize);
            }
            break;
        }
        case MsgType::ACK: {
            if (h.length == sizeof(uint32_t) + sizeof(uint16_t)) {
                uint32_t remoteSize;
                std::memcpy(&remoteSize, payload, sizeof(uint32_t));
                uint16_t remoteListeningPort;
                std::memcpy(&remoteListeningPort, payload + sizeof(uint32_t), sizeof(uint16_t));
                if (remoteSize > blockchain_.size())
                    requestBlocks(peer, remoteSize);
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
                if(blockchain_.addBlock(b)) {
                    if(not finishedRetrieve_){
                        finishedRetrieve_ = false;
                        semaphore2_.release();
                    }
                } else {
                    if(not finishedRetrieve_){
                        finishedRetrieve_ = true;
                        semaphore2_.release();
                    }
                }
            } catch(...) {
                //finished retrieve = true (en cas d'erreur)
                finishedRetrieve_ = true;
                //release semaphore (2)
                semaphore2_.release();
            }
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

void NodeNetwork::handleDisconnect(const PeerInfo& peer){
    peers_.erase(peer); // pour outgoing
    incoming_.erase(peer); // pour incoming
}

void NodeNetwork::buildAndSendFrame(const PeerInfo& peer, MsgType type, const std::vector<uint8_t>& payload){
    auto it = peers_.find(peer);
    if (it == peers_.end())
        return; //si le peer n'est pas trouvé
    auto frame = BinaryProtocol::buildFrame(type, payload);
    std::string s(reinterpret_cast<const char*>(frame.data()), frame.size());
    it->second->send(s);
}

void NodeNetwork::sendVersion(const PeerInfo& peer){
    uint32_t height = blockchain_.size();
    uint16_t listeningPort = listenPort_;
    auto payload = std::vector<uint8_t>(sizeof(height) + sizeof(listeningPort));
    std::memcpy(payload.data(), &height, sizeof(height));
    std::memcpy(payload.data() + sizeof(height), &listeningPort, sizeof(listeningPort));
    buildAndSendFrame(peer, MsgType::VERSION, payload);
}

void NodeNetwork::sendAck(const PeerInfo& peer){
    uint32_t height = blockchain_.size();
    uint16_t listeningPort = listenPort_;
    auto payload = std::vector<uint8_t>(sizeof(height) + sizeof(listeningPort));
    std::memcpy(payload.data(), &height, sizeof(height));
    std::memcpy(payload.data() + sizeof(height), &listeningPort, sizeof(listeningPort));
    buildAndSendFrame(peer, MsgType::ACK, payload);
}

void NodeNetwork::sendBlock(const PeerInfo& peer, uint32_t blockIdx){
    const auto& block = blockchain_[blockIdx];
    auto payload = BinaryProtocol::serializeObject<Block>(block);
    buildAndSendFrame(peer, MsgType::BLOCK, payload);
    
}

void NodeNetwork::requestBlock(const PeerInfo& peer, uint32_t blockIdx){
    auto payload = std::vector<uint8_t>(sizeof(blockIdx));
    std::memcpy(payload.data(), &blockIdx, sizeof(blockIdx));
    buildAndSendFrame(peer, MsgType::GET_BLOCK, payload);
}

void NodeNetwork::requestBlocks(const PeerInfo& peer, uint32_t remoteSize){
    semaphore1_.acquire();
    
    finishedRetrieve_ = false;
    
    for(uint32_t i = blockchain_.size(); i < remoteSize; ++i){
        requestBlock(peer, i);

        // Attendre la réponse avec un timeout de 7 secondes
        if(semaphore2_.try_acquire_for(std::chrono::seconds(7))) {
            // Bloc reçu dans les temps
            if(finishedRetrieve_) {
                break;
            }
        } else {
            // Timeout - le peer ne répond pas
            std::cout << "Timeout lors de la demande du bloc " << i << " au peer " << peer.getIp() << std::endl;
            finishedRetrieve_ = true;
            semaphore2_.release();
            break;
        }
    }
    semaphore1_.release();
}