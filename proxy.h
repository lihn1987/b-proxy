#ifndef PROXY_H
#define PROXY_H

#include <string>
#include <boost/thread.hpp>
#include <boost/signals2/signal.hpp>


struct ProxyConfig {
    std::string protocol = "vless";
    std::string id = "183f8067-a47a-3258-2cad-a0361b9f67a0";
    std::string address = "sharkmine.network";
    uint32_t port = 443;
    std::string security = "tls";
    std::string encryption = "none";
    std::string host = "sharkmine.network";
    std::string nick_name = "sharkmine.network_VLESS_TLS-direct_TCP";
};

struct ProxyLocalConfig {
    uint32_t enable;
    std::string server;
};

class Proxy
{
public:
    Proxy();
public:
    // 进行连接
    void Connect();

    // 断开连接
    void Disconnect();
public:
    // 将proxy_config中的配置同步到 v2ray中
    void SyncV2RayConfig();

    // 启动v2ray服务
    void StartV2RayService();

    //停止v2ray服务
    void StopV2RayService();

    // 设置操作系统代理设置
    void SetSystemConfig();

    // 清除操作系统代理设置
    void ClearSystemConfig();
public:
    boost::signals2::connection AddV2RayLogCallBack( boost::function<void(const std::string&)> cb);
    boost::signals2::connection AddCommonLogCallBack( boost::function<void(const std::string&)> cb);
private:
    void V2RayThreadFunc();
    void KillV2Ray();
    std::vector<std::pair<uint32_t/*pid*/, std::string/*process_name*/>> GetAllProcess();
public:
    // 获取本地代理
    bool GetProxyLocalConfig(ProxyLocalConfig& config);
    // 设置本地代理
    void SetProxyLocalConfig(ProxyLocalConfig config);
private:
    ProxyConfig proxy_config;
    boost::thread v2ray_thread;
    boost::signals2::signal<void(const std::string&)> v2ray_log_sig;
    boost::signals2::signal<void(const std::string&)> common_log_sig;
};

#endif // PROXY_H
