#ifndef PROXY_H
#define PROXY_H

#include <string>
#include <boost/thread.hpp>
#include <boost/signals2/signal.hpp>

struct ProxyLocalConfig {
    uint32_t enable;
    std::string server;
};

class Proxy
{
public:
    Proxy();
public:
    // 获取本地代理
    bool GetProxyLocalConfig(ProxyLocalConfig& config);
    // 设置本地代理
    void SetProxyLocalConfig(ProxyLocalConfig config);
    // 清除操作系统代理设置
    void ClearSystemConfig();
public:

};

#endif // PROXY_H
