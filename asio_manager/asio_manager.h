#ifndef ASIO_MANAGER_H
#define ASIO_MANAGER_H

#include <boost/asio.hpp>
#include <memory>
#include <thread>

class AsioManager{
public:
    static std::shared_ptr<AsioManager> GetInstance();
public:
    ~AsioManager();
    void StartThreads();
    void StopThreads();
    boost::asio::io_service& GetIoService();
    uint32_t GetThreadCount();
private:
    AsioManager();
private:
    void ThreadFunc();
    uint32_t GetCoreCount();
private:
    static std::shared_ptr<AsioManager> impl;
    boost::asio::io_service ios;
    std::shared_ptr<boost::asio::io_service::work> ios_work;
    std::vector<std::thread> thread_list;
    bool thread_run;
};
#define GetAsioManager() AsioManager::GetInstance()
#endif // ASIO_MANAGER_H
