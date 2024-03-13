#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "EventLoopThread.h"
#include <cstdio>
#include <memory>
#include <vector>

namespace fst {

EventLoopThreadPool::EventLoopThreadPool(EventLoop*baseLoop, const std::string &nameArg)
    : baseLoop_(baseLoop)
    , name_(nameArg)
    , started_(false)
    , numThreads_(0)
    , next_(0){}

void EventLoopThreadPool::start(const ThreadInitCallBack &cb){
    started_ = true;
    for(int i = 0; i < numThreads_; i++){
        char buf[name_.size() + 32];
        snprintf(buf, sizeof(buf), "%s%d", name_.c_str(), i);

        EventLoopThread * t = new EventLoopThread(cb,buf);      //创建子线程和subloop
        threads.push_back(std::unique_ptr<EventLoopThread>(t));
        loops_.push_back(t->startLoop());                           //启动子线程和loop循环
    }
    if(numThreads_ == 0 && cb){
        cb(baseLoop_);                  //只有主线程时
    }
}

EventLoop* EventLoopThreadPool::getNextLoop(){
    EventLoop* loop = baseLoop_;
    if(!loops_.empty()){
        loop = loops_[next_];
        ++next_;
        if(next_ >= loops_.size()){
            next_ = 0;
        }
    }
    return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops(){
    if(loops_.empty()){
        return std::vector<EventLoop*> (1, baseLoop_);
    }else{
        return loops_;
    }
}

}