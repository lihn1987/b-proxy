#include "server_socket_item.h"
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/format.hpp>
#include <boost/beast/http.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <sstream>
#include "../asio_manager/asio_manager.h"
#include "../crypto_tools/key_tools.h"
#include "../log/log.h"


ServerSocketItem::ServerSocketItem(){
    GetProxyLog()->LogDebug("创建一个链接");
    websocket = std::make_shared<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>>(GetAsioManager()->GetIoService());
    socket = std::make_shared<boost::asio::ip::tcp::socket>(GetAsioManager()->GetIoService());
}

ServerSocketItem::~ServerSocketItem(){
    Clear();
    GetProxyLog()->LogDebug("析构一个链接");
}

void ServerSocketItem::Connect(const boost::asio::ip::tcp::endpoint& end_point, const std::string& path) {
    try{
        websocket->next_layer().connect(end_point);
        std::string host = end_point.address().to_string() + (boost::format("/%d")%end_point.port()).str();
        
        websocket->set_option(boost::beast::websocket::stream_base::decorator([](boost::beast::websocket::request_type&req){
            req.set(boost::beast::http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING)+" websocket-client-coro");
        }));
        websocket->handshake(host, path);
        websocket->binary(true);
        on_connect();
        WebsocketReadFunc();
    } catch(...) {
        try{
            websocket->next_layer().close();
            SendError(shared_from_this());
            return;
        }catch(...){
            SendError(shared_from_this());
            return;
        }
    }
}

void ServerSocketItem::Accept(std::shared_ptr<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> ws, const std::string& path){
    try{
        this->websocket = ws;
        boost::beast::flat_buffer buffer;
        boost::beast::http::request<boost::beast::http::dynamic_body> req;
        boost::beast::http::read(ws->next_layer(), buffer, req);
        std::string req_path = req.target().to_string();
        if (path != req_path) {
            ws->next_layer().close();
            SendError(shared_from_this());
            return;
        }
        ws->accept(req);
        websocket->binary(true);
        on_connect();
    }catch(...){
        ws->next_layer().close();
    }
    
    WebsocketReadFunc();
}

bool ServerSocketItem::StartWithConnect(const boost::asio::ip::tcp::endpoint& end_point, const std::string& path){
    Clear();
    websocket_thread_func = std::thread(std::bind(&ServerSocketItem::Connect, this, end_point, path));
    return true;
}

bool ServerSocketItem::ConnectAsync(const boost::asio::ip::tcp::endpoint &end_point, const std::string &path)
{
    try{
        websocket->next_layer().connect(end_point);
        std::string host = end_point.address().to_string() + (boost::format("/%d")%end_point.port()).str();
        websocket->set_option(boost::beast::websocket::stream_base::decorator([](boost::beast::websocket::request_type&req){
            req.set(boost::beast::http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING)+" websocket-client-coro");
        }));
        websocket->handshake(host, path);
        
        websocket_thread_func = std::thread(std::bind(&ServerSocketItem::websocket_thread_func, shared_from_this()));

    } catch(...) {
        try{
            websocket->next_layer().close();
            SendError(shared_from_this());
            return false;
        }catch(...){
            SendError(shared_from_this());
            return false;
        }
    }
    return true;
}
bool ServerSocketItem::StartWithAccept(std::shared_ptr<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> ws, const std::string& path){
    Clear();
    websocket_thread_func = std::thread(std::bind(&ServerSocketItem::Accept, this, ws, path));
    return true;
}

bool ServerSocketItem::WebsocketWrite(const std::string& buf) {
    try{
        std::string encoded;
        AesEncode("12345678901234567890123456789012", "1234567890123456", buf, encoded);
        uint32_t size = websocket->write(boost::asio::buffer(encoded));

    }catch(...){
        SendError(shared_from_this());
        return false;
    }
    return true;
}

void ServerSocketItem::Clear(){
    std::lock_guard<std::mutex> lk(mutex);
    boost::system::error_code err;
    if(socket) {
        socket->cancel(err);
        socket->close(err);

    }
    if (socket_thread_func.joinable()) {
        socket_thread_func.join();
    }

    if(websocket){
        websocket->next_layer().close(err);
    }
    if (websocket_thread_func.joinable()) {
        websocket_thread_func.join();
    }


}

void ServerSocketItem::SetOnErrorCallback(std::function<void(std::shared_ptr<ServerSocketItem>)> cb){
    on_error = cb;
}

void ServerSocketItem::WebsocketReadFunc(){
    
    try{
        while(true) {
            boost::beast::flat_buffer flat_buf;
            uint32_t size = websocket->read(flat_buf);
            std::string buf = std::string((char*)boost::asio::buffer(flat_buf.data()).data(), flat_buf.size());
            std::string decoded_buf;
            AesDecode("12345678901234567890123456789012", "1234567890123456",buf , decoded_buf);
            // GetProxyLog()->LogDebug("收到数据:"+std::string(buf.data(), size));
            if (b_first){
                readed += decoded_buf;

                Decode();
            }else {
                boost::asio::write(*socket, boost::asio::buffer(decoded_buf));
            }
            
        }
    }catch(const boost::system::system_error& err){
        GetProxyLog()->LogDebug("连接断开:"+std::string(err.what()));
        SendError(shared_from_this());
    }
    catch(...){
        GetProxyLog()->LogDebug("连接断开:");
        SendError(shared_from_this());
    }
}

void ServerSocketItem::SocketReadFunc(){
    std::string buf;
    buf.resize(1024);
    try {
        while(true) {
            uint32_t size = socket->read_some(boost::asio::buffer(buf));
            // GetProxyLog()->LogDebug("socket收到数据:"+host+std::string(buf.data(), size));
            bool bbb = websocket->binary();
            WebsocketWrite(std::string(buf.data(), size));
        }
    }catch(...){
        SendError(shared_from_this());
    }
}
void ServerSocketItem::SendError(std::shared_ptr<ServerSocketItem> client){
    if(on_error)
        std::thread(std::bind(on_error, client)).detach();
}

void ServerSocketItem::Decode() {
    size_t pos = readed.find("\r\n\r\n");
    if (pos == -1){
        return;
    }
    b_first = false;
    try{
        /*
        std::vector<std::string> str_list;
        boost::split(str_list, readed, boost::is_any_of("\r\n"), boost::token_compress_on);
        std::string host_str = "";
        for (auto line: str_list) {
            if (boost::algorithm::starts_with(line, "Host")) {
                host_str = std::string(line, 5, line.length() - 5);
                boost::trim(host_str);
            }
        }*/
        std::vector<std::string> str_list;
        boost::split(str_list, readed, boost::is_any_of("\r\n"), boost::token_compress_on);
        if(str_list.size() < 1) throw "";
        std::string line1 = str_list[0];
        boost::split(str_list, line1, boost::is_any_of(" "), boost::token_compress_on);
        if (str_list.size() < 2) throw "";

        std::string cmd = str_list[0];
        std::string target = str_list[1];
        
        pos = target.find("//");
        if (pos != -1) {
            target = std::string(target, pos+2, target.length() - pos - 2);
        }
        boost::erase_last(target, "/");
        boost::split(str_list, target, boost::is_any_of(":"));
        std::string host = str_list[0];
        std::string port;

        if ( str_list.size() >= 2 ) 
            port = str_list[1];
        else 
            port = "80";
        boost::asio::ip::tcp::resolver resolver(GetAsioManager()->GetIoService());
        auto result = resolver.resolve(host, port);
//        boost::asio::connect(*socket, result);
        socket->connect(result->endpoint());
        
        if (cmd == "CONNECT"){
            //https代理
            GetProxyLog()->LogDebug("开始代理HTTPS:"+target);
            WebsocketWrite("HTTP/1.0 200 Connection established\r\n\r\n");
        }else{
            GetProxyLog()->LogDebug("开始代理HTTP:"+target);
            boost::asio::write(*socket, boost::asio::buffer(readed, readed.size()));
        }
        this->host = host;
        readed.clear();
        socket_thread_func = std::thread(std::bind(&ServerSocketItem::SocketReadFunc, shared_from_this()));
    }catch(...){
        GetProxyLog()->LogDebug("无法进行代理:"+readed);
        SendError(shared_from_this());
    }

}
