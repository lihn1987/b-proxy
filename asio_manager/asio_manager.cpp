#include "asio_manager.h"
#include <thread>
#include <functional>
#include <boost/bind/bind.hpp>
#include <boost/format.hpp>
#include "../log/log.h"
std::shared_ptr<AsioManager> AsioManager::impl = nullptr;

std::shared_ptr<AsioManager> AsioManager::GetInstance(){
    if(!impl){
        impl = std::shared_ptr<AsioManager>(new AsioManager());
    }
    return impl;
}

AsioManager::~AsioManager(){
    //为控制释放时间，由外部停止
    StopThreads();
}

void AsioManager::StartThreads(){
    StopThreads();
    ios.reset();
    ios_work = std::make_shared<boost::asio::io_service::work>(ios);
    GetProxyLog()->LogDebug((boost::format("内核总数:%d")%GetCoreCount()).str());
    for(uint32_t idx = 0; idx < GetCoreCount(); idx++){
        thread_list.push_back(std::thread(std::bind(&AsioManager::ThreadFunc, this)));
    }
}

void AsioManager::StopThreads(){
    ios.stop();
    for(uint32_t i = 0; i < thread_list.size(); i++){
        try{
            if(thread_list[i].joinable())
                thread_list[i].join();
        }catch(std::system_error& ec){
            GetProxyLog()->LogTrace(ec.what());
        }catch(...){

        }
        GetProxyLog()->LogTrace("停止线程!");
    }
    thread_list.clear();
}

boost::asio::io_service &AsioManager::GetIoService(){
    return ios;
}

uint32_t AsioManager::GetThreadCount() {
    return thread_list.size();
}

AsioManager::AsioManager():thread_run(true){
    //StartThreads();
}

void AsioManager::ThreadFunc(){
    GetProxyLog()->LogTrace("ASIO 异步IO线程开始运行!");
    boost::system::error_code ec;
    ios.run(ec);
    // GetChatLog()->LogTrace("ASIO 异步IO线程结束运行!");
}

uint32_t AsioManager::GetCoreCount(){
     return 1;
    return std::thread::hardware_concurrency();
}
