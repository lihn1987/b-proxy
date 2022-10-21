#ifndef CLIENT_SOCKET_ITEM_H
#define CLIENT_SOCKET_ITEM_H

#include <boost/asio.hpp>
#include <string>
#include <mutex>
#include <boost/thread.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/signals2.hpp>
#include <boost/beast.hpp>
#include <list>
#include <vector>
#include <atomic>
class NetMessage;
struct NetMessageHead;
class WebSocketClient;
class ClientSocketItem: public std::enable_shared_from_this<ClientSocketItem>
{
public:
    ClientSocketItem(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    ~ClientSocketItem();
    void SetOnErrorCallBack(std::function<void(std::shared_ptr<ClientSocketItem>)> cb);
    void SendError(std::shared_ptr<ClientSocketItem> client);
    void Clear();
private:
    void SocketThreadFunc(const boost::asio::ip::tcp::endpoint &end_point, const std::string &path);
    void WebSocketThreadFunc();
    void SocketWrite(const std::string& buf);
    void WebSocketWrite(const std::string& buf);
private:
    std::shared_ptr<boost::asio::ip::tcp::socket> socket;
    std::shared_ptr<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> websocket;
    std::thread socket_thread_func;
    std::thread websocket_thread_func;
    std::string read_buf;
    std::string readed;
    std::function<void(std::shared_ptr<ClientSocketItem>)> on_error;

    std::mutex close_mutex;

};

#endif // CLIENT_SOCKET_ITEM_H
