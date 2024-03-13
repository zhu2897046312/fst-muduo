#include "EventLoopThread.h"
#include "EventLoop.h"
#include "Thread.h"
#include <condition_variable>
#include <mutex>

namespace fst {

EventLoopThread::EventLoopThread(
    const ThreadInitCallBack &cb ,
    const std::string &name)
    : loop_(nullptr)
    , exiting_(false)
    , thread_(std::bind(&EventLoopThread::threadFunc,this),name)
    , mutex_()
    , condition_()
    , callback_(std::move(cb)){}

EventLoopThread::~EventLoopThread(){
    exiting_ = true;
    if(loop_ != nullptr){
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop(){
    thread_.start();

    EventLoop *loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while ( loop_ == nullptr ) {
            condition_.wait(lock);
        }
        loop = loop_;
    }

    return loop;
}

void EventLoopThread::threadFunc(){
    EventLoop loop;     //新线程 创建一个新的 EentLoop

    //所有线程回调都是这个cb ,真正用户想做的都在EventLoop
    if(callback_){
        callback_(&loop);   //loop做用户想做的事情
    }

    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        condition_.notify_one();
    }

    loop.loop();
    std::unique_lock<std::mutex> lock(mutex_);
    loop_ = nullptr;
}

}