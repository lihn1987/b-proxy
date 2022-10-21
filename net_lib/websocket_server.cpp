#include "websocket_server.h"
#include "server_socket_item.h"
#include "../log/log.h"
namespace websocket = boost::beast::websocket;
WebsocketServer::WebsocketServer(){
    
}

WebsocketServer::~WebsocketServer(){
    StopServer();
}

bool WebsocketServer::StartServer(boost::asio::io_service& ios, uint32_t port, const std::string& path){
    try{
        this->path = path;
        acceptor = std::shared_ptr<boost::asio::ip::tcp::acceptor>(
            new boost::asio::ip::tcp::acceptor(ios, {boost::asio::ip::make_address("0.0.0.0"), port})
        );
        thread_func = std::thread(std::bind(&WebsocketServer::ThreadFunc, this));
    }catch(...){
        return false;
    }
    return true;
}

bool WebsocketServer::StopServer(){
    if (acceptor) {
        try{
            acceptor->close();
        }catch(...){
            return false;
        }
    }
    return true;
}

void WebsocketServer::ThreadFunc(){
    while(true) {
        boost::asio::ip::tcp::socket socket =  acceptor->accept();
        auto ws = std::shared_ptr<websocket::stream<boost::asio::ip::tcp::socket>>(new websocket::stream<boost::asio::ip::tcp::socket>(std::move(socket)));
        ws->set_option(websocket::stream_base::decorator([](websocket::request_type& res){
            res.set(boost::beast::http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-sync");
        }));
        GetProxyLog()->LogDebug("收到新连接");
        std::shared_ptr<ServerSocketItem> client = std::make_shared<ServerSocketItem>();
        client->SetOnErrorCallback(std::bind(&WebsocketServer::OnClientError, this, std::placeholders::_1));
        client->StartWithAccept(ws, path);
        {
            std::lock_guard<std::mutex> lk(websocket_map_mutex);
            websocket_map.insert(client);
        }
    }
}

void WebsocketServer::OnClientError(std::shared_ptr<ServerSocketItem> ws){
    // std::thread(std::bind(&WebSocketClient::Clear, ws)).detach();
    ws->Clear();
    {
        std::lock_guard<std::mutex> lk(websocket_map_mutex);
        websocket_map.erase(ws);
    }
    GetProxyLog()->LogDebug("服务端删除一个链接");
}