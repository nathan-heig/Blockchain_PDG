// TcpConnection.hpp (version simplifiée: SimpleConnection)
#ifndef SIMPLE_CONNECTION_HPP
#define SIMPLE_CONNECTION_HPP

#include <boost/asio.hpp>
#include <memory>
#include <deque>
#include <functional>
#include <cstring>
#include <array>

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    using Socket = boost::asio::ip::tcp::socket;
    using OnMessage = std::function<void(const std::string&)>;
    using OnClose   = std::function<void()>;

    explicit TcpConnection(boost::asio::io_context& ctx)
        : socket(ctx) {}

    void start(OnMessage onMsg, OnClose onCloseCb) {
        onMessage = std::move(onMsg);
        onClose   = std::move(onCloseCb);
        readHeader();
    }
    
    Socket& socketRef() { return socket; }

    void send(const std::string& payload) {
        // Frame = [4 octets longueur][données]
        uint32_t len = static_cast<uint32_t>(payload.size());
        std::string frame(sizeof(uint32_t), '\0');
        std::memcpy(frame.data(), &len, sizeof(uint32_t));
        frame += payload;

        bool writing = !outQueue.empty();
        outQueue.push_back(std::move(frame));
        if (!writing) writeNext();
    }

private:
    Socket socket;
    std::array<char, 4> hdrBuf_{};
    std::string bodyBuf;
    std::deque<std::string> outQueue;
    OnMessage onMessage;
    OnClose onClose;
    static constexpr uint32_t kMax = 1024 * 1024; // 1MB

    void readHeader() {
        auto self = shared_from_this();
        boost::asio::async_read(socket, boost::asio::buffer(hdrBuf_),
            [this, self](auto ec, std::size_t){
                if (ec) return close();
                uint32_t len = 0;
                std::memcpy(&len, hdrBuf_.data(), 4);
                if (len > kMax){
                    return close();
                }
                bodyBuf.assign(len, '\0');
                readBody(len);
            });
    }
    void readBody(uint32_t len) {
        auto self = shared_from_this();
        boost::asio::async_read(socket, boost::asio::buffer(bodyBuf.data(), len),
            [this, self](auto ec, std::size_t){
                if (ec){
                    return close();
                }
                if (onMessage){
                    onMessage(bodyBuf);
                }
                readHeader(); // boucle
            });
    }
    void writeNext() {
        auto self = shared_from_this();
        boost::asio::async_write(socket, boost::asio::buffer(outQueue.front()),
            [this, self](auto ec, std::size_t){
                if (ec) return close();
                outQueue.pop_front();
                if (!outQueue.empty()) writeNext();
            });
    }
    void close() {
        boost::system::error_code ig;
        socket.close(ig);
        if (onClose) onClose();
    }
};

#endif // SIMPLE_CONNECTION_HPP