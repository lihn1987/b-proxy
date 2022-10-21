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

void Proxy::ClearSystemConfig(){
    ProxyLocalConfig config;
    config.enable = 0;
    config.server = "127.0.0.1:1082";
    SetProxyLocalConfig(config);
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
