#ifndef LOG_H
#define LOG_H

#include <memory>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
class ProxyLog
{
public:
    static std::shared_ptr<ProxyLog> GetInstance();
    void LogTrace(const std::string& log);
    void LogDebug(const std::string& log);
    void LogInfo(const std::string& log);
    void LogError(const std::string& log);
    void LogFatal(const std::string& log);
    void Log(const std::string& log, boost::log::trivial::severity_level log_level);
private:
    ProxyLog();
    void Init(boost::log::trivial::severity_level log_level);
private:
    static std::shared_ptr<ProxyLog> impl_;
};
#define GetProxyLog() ProxyLog::GetInstance()

#endif // LOG_H
