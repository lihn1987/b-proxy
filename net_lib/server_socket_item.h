#include <memory>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <boost/signals2/signal.hpp>

class ServerSocketItem: public std::enable_shared_from_this<ServerSocketItem>{
public:
    ServerSocketItem();
    ~ServerSocketItem();
    bool StartWithConnect(const boost::asio::ip::tcp::endpoint& end_point, const std::string& path);
    bool ConnectAsync(const boost::asio::ip::tcp::endpoint& end_point, const std::string& path);
    bool StartWithAccept(std::shared_ptr<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> ws, const std::string& path);
    bool WebsocketWrite(const std::string& buf);
    void Clear();
public:
    void SetOnErrorCallback(std::function<void(std::shared_ptr<ServerSocketItem>)> cb);
private:
    void Connect(const boost::asio::ip::tcp::endpoint& end_point, const std::string& path);
    void Accept(std::shared_ptr<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> ws, const std::string& path);
    void WebsocketReadFunc();
    void SocketReadFunc();
    void SendError(std::shared_ptr<ServerSocketItem> client);

    void Decode();
private:
    std::shared_ptr<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> websocket;
    std::shared_ptr<boost::asio::ip::tcp::socket> socket;
    std::thread websocket_thread_func;
    std::thread socket_thread_func;
    std::mutex mutex;
    std::function<void(std::shared_ptr<ServerSocketItem>)> on_error;
    bool b_first = true;
    std::string readed;
    boost::signals2::signal<void()> on_connect;
    boost::signals2::signal<void(const std::string&)> on_read;
    std::string host;
};
