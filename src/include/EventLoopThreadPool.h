#pragma once

#include "Channel.h"
#include "EventLoopThread.h"
#include "noncopyable.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>
namespace fst {

class EventLoopThreadPool : nonccopyable
{
public:
    using ThreadInitCallBack = std::function<void(EventLoop*)>;

    EventLoopThreadPool(EventLoop*baseLoop, const std::string &nameArg);
    ~EventLoopThreadPool() = default;

    void setThreadNum(int numThreads) { numThreads_ = numThreads; }

    /**
        1. 创建子线程和subloop
        2. 启动子线程和loop循环
        3. 只有主线程时 cb(baseloop)
    */
    void start(const ThreadInitCallBack &cb = ThreadInitCallBack() );

    EventLoop* getNextLoop();

    std::vector<EventLoop*> getAllLoops();

    bool started() const { return started_; }
    const std::string name() const { return name_; }
private:

    EventLoop *baseLoop_;
    std::string name_;
    bool started_;
    int numThreads_;
    int next_;
    std::vector<std::unique_ptr<EventLoopThread>> threads;
    std::vector<EventLoop*> loops_;
};

}