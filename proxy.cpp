#include "proxy.h"
#include <boost/bind/bind.hpp>
#include <boost/process.hpp>
#include <boost/process/windows.hpp>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <boost/format.hpp>
#include <QDebug>
#include <Windows.h>

bool ReadReg(HKEY key, const char *path, const char* key_name, char *buf);
bool SetReg(HKEY key, const char *path, const char* key_name, DWORD type, char *buf, uint32_t size);
Proxy::Proxy()
{

}

void Proxy::Connect(){
    SyncV2RayConfig();
    StartV2RayService();
}

void Proxy::Disconnect(){
    StopV2RayService();
}

void Proxy::SyncV2RayConfig()
{

}

void Proxy::StartV2RayService(){
    SyncV2RayConfig();
    v2ray_thread = boost::thread(boost::bind(&Proxy::V2RayThreadFunc, this));
    SetSystemConfig();
}

void Proxy::StopV2RayService(){
    KillV2Ray();
    v2ray_thread.join();
    ClearSystemConfig();
}

void Proxy::SetSystemConfig(){
    ProxyLocalConfig config;
    config.enable = 1;
    config.server = "127.0.0.1:1082";
    SetProxyLocalConfig(config);
}

void Proxy::ClearSystemConfig(){
    ProxyLocalConfig config;
    config.enable = 0;
    config.server = "127.0.0.1:1082";
    SetProxyLocalConfig(config);
}

boost::signals2::connection Proxy::AddV2RayLogCallBack(boost::function<void (const std::string &)> cb){
    return v2ray_log_sig.connect(cb);
}

boost::signals2::connection Proxy::AddCommonLogCallBack(boost::function<void (const std::string &)> cb){
    return common_log_sig.connect(cb);
}

void Proxy::V2RayThreadFunc(){
    std::string str_res;
    std::error_code ec;
    boost::process::ipstream is;
    boost::process::ipstream is2;

    std::string cmd = ".\\v2ray-core\\v2ray.exe";
    boost::process::child process = boost::process::child(cmd, boost::process::std_out>is, boost::process::std_err>is2, boost::process::windows::create_no_window);

    if(!ec){
        while(!is.eof()) {
            char buf[1025] = {0};
            is.getline(buf, 1024);
            v2ray_log_sig(buf);
        }
    } else {
        std::string xxx = ec.message();
        qDebug()<<QString::fromLocal8Bit(xxx.c_str());
        qDebug()<<QString::fromStdString(xxx.c_str());
        qDebug()<<QString::fromUtf8(xxx.c_str());
        std::cout<< ec.message()<< std::endl;
    }
}

void Proxy::KillV2Ray(){
    boost::process::system("taskkill /F /IM v2ray.exe /T", boost::process::windows::create_no_window);
    v2ray_log_sig("停止proxy服务...");
}

std::vector<std::pair<uint32_t, std::string> > Proxy::GetAllProcess(){
    std::string str_res;
    std::error_code ec;
    //boost::process::ipstream is;
    std::ifstream is;
    std::string cmd = "tasklist";
    std::vector< std::pair<uint32_t, std::string> > rtn;
    boost::process::system(cmd, boost::process::std_out>"tasklist.cfg", boost::process::windows::create_no_window);

    if(!ec){
        is.open("tasklist.cfg");
        if(is.is_open()){
            std::string cmd_out = std::string(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
            std::vector<std::string> lines;
            boost::split(lines, cmd_out, boost::is_any_of("\n"));
            for(uint32_t i = 3; i < lines.size(); i++) {
                std::string line = lines[i];
                if (line.size() >= 34){
                    std::string name = boost::algorithm::trim_copy(std::string(line.begin(), line.begin()+25));
                    std::string pid = boost::algorithm::trim_copy(std::string(line.begin()+26, line.begin()+34));
                    rtn.push_back({(uint32_t)atoi(pid.c_str()), name});
                    qDebug()<<pid.c_str()<<name.c_str();
                }
            }
        }
    } else {

    }
    return rtn;
}

bool Proxy::GetProxyLocalConfig(ProxyLocalConfig& config){
    uint64_t enable_value = 0;
    ReadReg(HKEY_CURRENT_USER,
            "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\",
            "ProxyEnable",
            (char*)&(config.enable)
            );

    char server_buf[1024] = {0};
    ReadReg(HKEY_CURRENT_USER,
            "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\",
            "ProxyServer",
            server_buf
            );
    config.server = server_buf;
    return true;
}

void Proxy::SetProxyLocalConfig(ProxyLocalConfig config){

    SetReg(HKEY_CURRENT_USER,
            "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\",
            "ProxyEnable",
            REG_DWORD,
            (char*)&(config.enable),
            4
    );
    SetReg(HKEY_CURRENT_USER,
            "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\",
            "ProxyServer",
            REG_SZ,
            (char*)(config.server.data()),
            config.server.length()
    );
}

bool ReadReg(HKEY key, const char *path, const char* key_name, char *buf){
    HKEY handle = NULL;
    if(ERROR_SUCCESS != RegOpenKeyExA(
        key,
        path,
        0,
        KEY_READ,
        &handle
                )){
        return false;
    };
    DWORD key_type;
    DWORD key_size = 1024;

    if(ERROR_SUCCESS != RegQueryValueExA(
        handle,
        key_name,
        0,
        &key_type,
        (LPBYTE)buf,
        &key_size)){
        return false;
    }
    RegCloseKey(handle);
    return true;
}
bool SetReg(HKEY key, const char *path, const char* key_name, DWORD type, char *buf, uint32_t size){
    HKEY handle = NULL;
    if(ERROR_SUCCESS != RegOpenKeyExA(
        key,
        path,
        0,
        KEY_WRITE,
        &handle
                )){
        return false;
    };
    RegSetValueExA(
        handle,
        key_name,
        0,
        type,
        (LPBYTE)buf,
        size
    );
}
