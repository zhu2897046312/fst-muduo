#include "Logger.h"

#include "Timestamp.h"

#include <iostream>

namespace fst {
//获取实例对象
Logger& Logger::GetInstance(){
    static Logger logger;
    return logger;
}
    //设置日志级别
void Logger::setLogLevel(LogLevel level){
    logLevel_ = level;
}
    //写日志
void Logger::log(std::string msg){
    switch (logLevel_) {
    case INFO:
        std::cout << "[INFO]";
        break;
    case ERROR:
        std::cout << "[ERRPR]";
        break;
    case FATAL:
        std::cout << "[FATAL]";
        break;
    case DEBUG:
        std::cout << "[DEBUG]";
        break;
    default:
        break;
    }

    std::cout << Timestamp::now().toString() << " : " << msg << std::endl;
}

}


