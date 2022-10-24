#ifndef CLIENT_SERVER_H
#define CLIENT_SERVER_H

#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <memory>
#include <list>
#include <unordered_set>
#include <mutex>
#include "client_socket_item.h"
class ClientSocketPair;
class ClientServer{
public:
    ClientServer();
    ~ClientServer();
public:
    bool StartServer(boost::asio::io_service& ios, bool auto_proxy, uint32_t port = 1011);
    bool StopServer();
private:
    std::thread thread_func;
    void ThreadFunc(boost::asio::io_service* ios, bool auto_proxy, uint32_t port);
    void OnError(std::shared_ptr<ClientSocketItem> item);
private:
    std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor;
    std::mutex socket_list_mutex;
    std::unordered_set<std::shared_ptr<ClientSocketItem>> socket_list;

};

#endif // CLIENT_SERVER_H
