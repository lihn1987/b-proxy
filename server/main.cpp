
#include "../asio_manager/asio_manager.h"
#include "../net_lib/websocket_server.h"
#include <boost/thread.hpp>
int main() {
    WebsocketServer* server = new WebsocketServer();
    server->StartServer(GetAsioManager()->GetIoService());
    // getchar();
    while(true){
        boost::this_thread::sleep(boost::posix_time::seconds(1));
    }
    return 0;
}