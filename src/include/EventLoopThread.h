#pragma once

#include "EventLoop.h"
#include "noncopyable.h"
#include "Thread.h"
#include <condition_variable>
#include <functional>
#include <mutex>
namespace fst {

/**
    将 一个EventLoop和一个Thread绑定到一起  实现 one Loop per Thread
*/
class EventLoopThread : nonccopyable
{

public:
    using ThreadInitCallBack = std::function<void(EventLoop*)>;
    
    EventLoopThread(const ThreadInitCallBack &cb = ThreadInitCallBack()
                    , const std::string &name = std::string());
    ~EventLoopThread();

    /**
        1. 启动一个线程
        2. loop创建好后,唤醒该线程
        
    */
    EventLoop* startLoop();
private:
    /**
        1. 创建一个新loop
        2. 通知等待的线程
        3. 执行loop循环
     */
    void threadFunc();
private:

    EventLoop *loop_;
    bool exiting_;
    std::mutex mutex_;
    Thread thread_;
    std::condition_variable condition_;
    ThreadInitCallBack callback_;
};

}