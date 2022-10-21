#include <memory>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <list>
#include <unordered_set>
class ServerSocketItem;
class WebsocketServer {
public:
    WebsocketServer();
    ~WebsocketServer();
public:
    bool StartServer(boost::asio::io_service& ios, uint32_t port = 9922, const std::string& path = "/test");
    bool StopServer();
private:
    void ThreadFunc();
    void OnClientError(std::shared_ptr<ServerSocketItem> ws);
private:
    std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor;
    std::thread thread_func;
    std::string path;
    std::mutex websocket_map_mutex;
    std::unordered_set<std::shared_ptr<ServerSocketItem>> websocket_map;
};