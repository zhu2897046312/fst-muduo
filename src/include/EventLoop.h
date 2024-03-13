#pragma once

#include "Channel.h"
#include "CurrentThread.h"
#include "Poller.h"
#include "Timestamp.h"
#include "noncopyable.h"

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <sched.h>
#include <vector>

namespace fst {

class Channel;
class Poller;
/**
    Reactor
    1. 事件循环开启和关闭
    2. channel->fd() 上有事件发生 从epoll_wait()返回 执行对应的fd读写、关闭操作
    3. event_fd上有写事件发生 通过此操作来唤醒被阻塞的线程 执行相应的回调操作

*/
class EventLoop : nonccopyable
{
public:
    using Func = std::function<void()>;
    EventLoop();
    ~EventLoop();
    //开启事件循环
    void loop();
    //关闭事件循环
    void quit();

    Timestamp pollReturnTime() const { return pollReturnTime_; }
    //在当前loop中执行cb
    void runInLoop(Func cb);
    //把cb放入队列中,唤醒loop所在线程,执行cb
    void queueInLoop(Func cb);
    //唤醒loop所在线程
    void wakeUp();

    void handleRead();
    void doPendingFunctors();

    // EventLoop  调用 ==> Poller 中的updateChannel 和removeChannel
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);
    //判断EventLoop对象是否在创建自己的线程中
    bool isInLoopThread() const {return thread_id_ == GetCurrentThreadId();}
private:
    using ChannelList = std::vector<Channel*>;

    std::atomic_bool looping_;
    std::atomic<bool> quit_;                    //标识退出loop循环
    
    const pid_t thread_id_;                     //判断创建loop所在线程id
    Timestamp pollReturnTime_;                  // poller返回事件的channels的时间点
    std::unique_ptr<Poller> poller_;

    /**
     * 当mainloop获取一个新用户的channel,
     * 通过轮询算法选择一个subloop,
     * 通过该成员变量唤醒subloop处理channel。
     */
    int wakeUpFd_;
    std::unique_ptr<Channel> wakeUpChannel_;

    ChannelList activeChannels_;
    Channel* currentActiveChannel_; 

    std::atomic_bool callingPendingFunctors_;    //标识当前loop是否需要执行回调操作
    std::vector<Func> pedingFunctors_;   //线程回调         //loop需要进行的所有回调操作- -- 该loop对应线程要做的事情（线程回调）
    std::mutex mutex_;
};

}