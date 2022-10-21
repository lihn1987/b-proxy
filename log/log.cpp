#include "log.h"

#include <boost/log/utility/setup/file.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <iostream>
#include <mutex>
#ifndef NO_QDEBUG
#include <QDebug>
#endif
std::shared_ptr<ProxyLog> ProxyLog::impl_ = nullptr;

std::shared_ptr<ProxyLog> ProxyLog::GetInstance(){
    if(!impl_) impl_ = std::shared_ptr<ProxyLog>(new ProxyLog());
    return impl_;
}

void ProxyLog::LogTrace(const std::string &log){
    Log(log, boost::log::trivial::trace);
}

void ProxyLog::LogDebug(const std::string &log){
    Log(log, boost::log::trivial::debug);
}

void ProxyLog::LogInfo(const std::string &log){
    Log(log, boost::log::trivial::info);
}

void ProxyLog::LogError(const std::string &log){
    Log(log, boost::log::trivial::error);
}

void ProxyLog::LogFatal(const std::string &log){
    Log(log, boost::log::trivial::fatal);
}

void ProxyLog::Log(const std::string &log, boost::log::trivial::severity_level log_level){
    // static std::mutex mutex;
    // std::lock_guard<std::mutex> lk(mutex);
    std::string str_level;
    std::string str_time;
    str_time = boost::posix_time::to_iso_string(boost::posix_time::microsec_clock::local_time());
    BOOST_LOG_STREAM_WITH_PARAMS(::boost::log::trivial::logger::get(),(::boost::log::keywords::severity = log_level))
            <<boost::log::trivial::to_string(log_level)<<"---"
            <<str_time<<"---"
            <<log;
#ifndef NO_QDEBUG
qDebug()<<boost::log::trivial::to_string(log_level)<<"---"
           <<QString::fromUtf8(str_time.c_str())<<"---"
           <<QString::fromUtf8(log.c_str());
#endif
    
}

ProxyLog::ProxyLog(){
    Init(boost::log::trivial::trace);
}

void ProxyLog::Init(boost::log::trivial::severity_level log_level){
    boost::log::add_console_log();
    //boost::log::add_file_log(boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time())+".log");
    boost::log::core::get()->set_filter(boost::log::trivial::severity>=log_level);
}
