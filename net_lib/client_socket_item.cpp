#include "client_socket_item.h"
#include <boost/thread/shared_lock_guard.hpp>
#include <boost/format.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast.hpp>
#include <boost/algorithm/string.hpp>
#include "../asio_manager/asio_manager.h"
#include "../crypto_tools/key_tools.h"
#include "../log/log.h"
#include "./pac.h"
const uint32_t BUFFER_SIZE = 1024;
ClientSocketItem::ClientSocketItem(std::shared_ptr<boost::asio::ip::tcp::socket> socket, bool auto_proxy):socket(socket){
    read_buf.resize(1024);
    websocket = std::shared_ptr<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>>(
               new boost::beast::websocket::stream<boost::asio::ip::tcp::socket>(GetAsioManager()->GetIoService()));
    socket_thread_func = std::thread(
                std::bind(
                    &ClientSocketItem::SocketThreadFunc,
                    this, "/test", auto_proxy));

}

ClientSocketItem::~ClientSocketItem(){
    Clear();
    GetProxyLog()->LogDebug("析构一个socket pair");
}

void ClientSocketItem::SetOnErrorCallBack(std::function<void (std::shared_ptr<ClientSocketItem>)> cb){
    on_error = cb;
}

void ClientSocketItem::SendError(std::shared_ptr<ClientSocketItem> client){
    if (on_error)
        std::thread(std::bind(on_error, client)).detach();
}

void ClientSocketItem::Clear(){

    std::lock_guard<std::mutex> lk(close_mutex);
    boost::system::error_code err;
    socket->close(err);
    if (socket_thread_func.joinable()) {
        try{socket_thread_func.join();}catch(...){}
        socket_thread_func = std::thread();
    }

    websocket->next_layer().close(err);

    if (websocket_thread_func.joinable()) {
        try{websocket_thread_func.join();}catch(...){}
        websocket_thread_func = std::thread();
    }
}

void ClientSocketItem::SocketThreadFunc(const std::string &path, bool auto_proxy){
    try{
        //读取第一条消息
        bool direct = false;
        while(auto_proxy) {
            uint32_t size = socket->read_some(boost::asio::buffer(read_buf, read_buf.size()));
            readed += std::string_view(read_buf.data(), size);
            std::size_t pos = readed.find("\r\n\r\n");
            if (pos != -1) {
                //判断域名是不是在pac中
                std::vector<std::string> str_list;
                boost::split(str_list, readed, boost::is_any_of("\r\n"), boost::token_compress_on);
                if(str_list.size() < 1) throw "";
                std::string line1 = str_list[0];
                boost::split(str_list, line1, boost::is_any_of(" "), boost::token_compress_on);
                if (str_list.size() < 2) throw "";

                std::string cmd = str_list[0];
                std::string target = str_list[1];

                boost::erase_first(target, "http://");
                boost::erase_first(target, "https://");
                boost::erase_first(target, "ws://");
                boost::erase_first(target, "wss://");
                pos = target.find("/");
                if (pos != -1) {
                    target = std::string(target, 0, pos);
                }
                boost::split(str_list, target, boost::is_any_of(":"));
                std::string host = str_list[0];
                std::vector<std::string> host_list;
                boost::split(host_list, host, boost::is_any_of("."));
                uint32_t host_list_size = host_list.size();
                if (host_list_size < 2)
                    throw "";
                host = host_list[host_list_size-2]+"."+host_list[host_list_size-1];
                if(pac_list.find(host) == pac_list.end()){
                    direct = true;
                }
                break;
            }
        }
        // 建立websocket通道
        boost::asio::ip::tcp::endpoint end_point;
        if (direct) {
            end_point = {boost::asio::ip::make_address("127.0.0.1"), 1012};
        }else{
            end_point = {boost::asio::ip::make_address("34.80.64.133"), 80};
        }
        websocket->next_layer().connect(end_point);
        std::string host = end_point.address().to_string() + (boost::format(":%d")%end_point.port()).str();
        websocket->set_option(boost::beast::websocket::stream_base::decorator([](boost::beast::websocket::request_type&req){
            req.set(boost::beast::http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING)+" websocket-client-coro");
        }));

        websocket->handshake(host, path);
        websocket->binary(true);
        websocket_thread_func = std::thread(std::bind(&ClientSocketItem::WebSocketThreadFunc, shared_from_this()));

        // ================
        // 进入消息读取循环

        //先把http头发出去
        WebSocketWrite(readed);
        readed.clear();
        while(true) {
            uint32_t size = socket->read_some(boost::asio::buffer(read_buf, read_buf.size()));
            //readed += std::string_view(read_buf.data(), size);
            //GetProxyLog()->LogDebug(std::string(read_buf.data(), size));
            WebSocketWrite(std::string(read_buf.data(), size));
            //readed.clear();
        }
    }catch(...) {
        SendError(shared_from_this());
    }
}

void ClientSocketItem::WebSocketThreadFunc(){
//    std::string buf;

    try{
        while(true) {
            boost::beast::flat_buffer buffer;
            uint32_t size = websocket->read(buffer);
            std::string encode_buf = std::string((char*)boost::asio::buffer(buffer.data()).data(), size);
            std::string decoded_buf;
            AesDecode("12345678901234567890123456789012", "1234567890123456",encode_buf, decoded_buf);
            GetProxyLog()->LogDebug("收到数据:"+decoded_buf);
            boost::asio::write(*socket, boost::asio::buffer(decoded_buf));
//            std::string readed;
//            readed.assign(buf.data(), size);
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

void ClientSocketItem::SocketWrite(const std::string &buf){
    boost::asio::write(*socket, boost::asio::buffer(buf));
}

void ClientSocketItem::WebSocketWrite(const std::string &buf){
    std::string encoded;
    bool x = websocket->binary();
    AesEncode("12345678901234567890123456789012", "1234567890123456", buf, encoded);
    websocket->write(boost::asio::buffer(encoded, encoded.size()));
}
