#include "client_server.h"
#include "../asio_manager/asio_manager.h"
#include "../log/log.h"
#include <boost/bind/bind.hpp>
#include <boost/format.hpp>
#include "../crypto_tools/key_tools.h"
ClientServer::ClientServer(){

}

ClientServer::~ClientServer(){
    StopServer();
}

bool ClientServer::StartServer(boost::asio::io_service& ios, uint32_t port){
    thread_func = std::thread(std::bind(&ClientServer::ThreadFunc, this, &ios, port));
    return true;
}

bool ClientServer::StopServer(){
    if (acceptor) {
        try{
            acceptor->close();
        }catch (...) {}
    }
    if (thread_func.joinable())
        thread_func.join();
    for(auto item: socket_list) {
        item->Clear();
    }
    return true;
}

void ClientServer::ThreadFunc(boost::asio::io_service* ios, uint32_t port){
    try {
        acceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(*ios, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
        GetProxyLog()->LogDebug("clietn_server is listening...");
        while(true){
            std::shared_ptr<boost::asio::ip::tcp::socket> socket = std::make_shared<boost::asio::ip::tcp::socket>(acceptor->accept());
            std::shared_ptr<ClientSocketItem> client = std::shared_ptr<ClientSocketItem>(new ClientSocketItem(socket));
            client->SetOnErrorCallBack(std::bind(&ClientServer::OnError, this, std::placeholders::_1));
            std::lock_guard<std::mutex> lk(socket_list_mutex);
            socket_list.insert(client);
        }
    }catch(...) {
        GetProxyLog()->LogDebug("clietn_server is break...");
    }
}

void ClientServer::OnError(std::shared_ptr<ClientSocketItem> item){
    item->Clear();
    std::lock_guard<std::mutex> lk(socket_list_mutex);
    socket_list.erase(item);
}
