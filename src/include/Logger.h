#pragma  once
#include "noncopyable.h"

#include <string>

// LOG_INFO("%s %d",arg1,agr2)
#define LOG_INFO(LogmsgFormat, ...) \
    do       \
    {           \
        fst::Logger& logger = fst::Logger::GetInstance(); \
        logger.setLogLevel(fst::INFO);   \
        char buf[1024] = {0};       \
        snprintf(buf,sizeof(buf), LogmsgFormat, ##__VA_ARGS__); \
        logger.log(buf);      \
    }while(0);  
#define LOG_ERROR(LogmsgFormat, ...) \
    do       \
    {           \
        fst::Logger& logger = fst::Logger::GetInstance(); \
        logger.setLogLevel(fst::ERROR);   \
        char buf[1024] = {0};       \
        snprintf(buf,sizeof(buf), LogmsgFormat, ##__VA_ARGS__); \
        logger.log(buf);      \
    }while(0); 
#define LOG_FATAL(LogmsgFormat, ...) \
    do       \
    {           \
        fst::Logger& logger = fst::Logger::GetInstance(); \
        logger.setLogLevel(fst::FATAL);   \
        char buf[1024] = {0};       \
        snprintf(buf,sizeof(buf), LogmsgFormat, ##__VA_ARGS__); \
        logger.log(buf);      \
    }while(0); 
    
#ifdef MUDEBUG 
#define LOG_DEBUG(LogmsgFormat, ...) \
    do       \
    {           \
        fst::Logger& logger = fst::Logger::GetInstance(); \
        logger.setLogLevel(fst::DEBUG);   \
        char buf[1024] = {0};       \
        snprintf(buf,sizeof(buf), LogmsgFormat, ##__VA_ARGS__); \
        logger.log(buf);      \
    }while(0);
#else
    #define LOG_DEBUG(LogmsgFormat, ...)
#endif

namespace fst {
enum LogLevel{
    INFO = 0,
    ERROR = 1,
    FATAL = 2,
    DEBUG = 3
};
class Logger : nonccopyable
{
public:
    //获取实例对象
    static Logger& GetInstance();
    //设置日志级别
    void setLogLevel(LogLevel level);
    //写日志
    void log(std::string msg);
private:
    LogLevel logLevel_;
    Logger(){}
};

}