#include "EventLoop.h"
#include "Channel.h"
#include "CurrentThread.h"
#include "Logger.h"

#include <cstdint>
#include <cstdlib>
#include <mutex>
#include <sys/eventfd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <vector>

namespace fst {

// 防止一个线程持有多个EventLoop 
static thread_local EventLoop* t_loopInThisThread = nullptr;

//默认的Poller IO复用接口的超时时间
const int kPollTimeMs = 10000;

//创建wakupfd,用来notify唤醒subloop执行回调从操作
int createEventfd(){
    int eventfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(eventfd < 0){
        LOG_FATAL("eventfd error:%d\n",errno);
        exit(EXIT_FAILURE);
    }
    return  eventfd; 
}

EventLoop::EventLoop()
    : looping_(false)
    , quit_(false)
    , callingPendingFunctors_(false)
    , thread_id_(GetCurrentThreadId())
    , poller_(Poller::newDefaltPoller(this))
    , wakeUpFd_(createEventfd())
    , wakeUpChannel_(new Channel(this, wakeUpFd_)){
    LOG_DEBUG("EventLoop Create %p in thread %d \n", this, thread_id_);
    if(t_loopInThisThread){
        LOG_FATAL("Another EventLoop %p exists in this thread %d\n", t_loopInThisThread, thread_id_);
    }else{
        t_loopInThisThread = this;
    }
    //设置wakeupfd感兴趣的事件和回调操作
    wakeUpChannel_->setReadCallBack(std::bind(&EventLoop::handleRead,this));
    //每一个eventloop都将监听wakeup的EPOLLIN事件    
    wakeUpChannel_->enableReading();    //设置监听事件
}

EventLoop::~EventLoop(){
    wakeUpChannel_->disableAll();
    wakeUpChannel_->remove();
    ::close(wakeUpFd_);
    t_loopInThisThread = nullptr;
}
//wakeUpFd_ 写了数据 为了唤醒subloop 再读一下数据
void EventLoop::handleRead(){
    uint64_t one = 1;
    ssize_t n = read(wakeUpFd_, &one, sizeof(one));
    if(n != sizeof(one)){
        LOG_ERROR("EventLoop::handleRead() reads %lu bytes instead 0f 8", n);
    }
}

//开启事件循环
void EventLoop::loop(){
    looping_ = true;
    quit_ = false;

    LOG_INFO("EventLoop %p start looping \n", this);

    while (!quit_) {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimeMs,&activeChannels_);
        //具体发生事件,执行回调
        for(Channel * channel : activeChannels_){
            channel->handleEvent(pollReturnTime_);
        }
        //执行当前Eventloop事件循环需要处理的回调操作
        /**
         IO线程 -- mainLoop 做accept接受新用户的连接 以连接用户需要分发给subLoop 

         wakeup 后需要执行的方法(wait出来后要干嘛！接收、发送数据等)
        */
        doPendingFunctors();    //做用户提交的任务
    }

    LOG_INFO("EventLoop %p stop looping \n",this);
    looping_ = false;
}
//关闭事件循环
void EventLoop::quit(){
    quit_ =true;
    if(!isInLoopThread()){
        wakeUp();
    }
}

//在当前loop中执行cb
void EventLoop::runInLoop(Func cb){
    if(isInLoopThread()){
        cb();
    }else{
        queueInLoop(cb);
    }
}
//把cb放入队列中,唤醒loop所在线程,执行cb
void EventLoop::queueInLoop(Func cb){
    {
 
    std::unique_lock<std::mutex> lock(mutex_);
    pedingFunctors_.emplace_back(cb);
           
    }
    if(!isInLoopThread() || callingPendingFunctors_){
        wakeUp();   //唤醒loop所在线程  -- 从epoll_wait() 出来
    }
}
//唤醒loop所在线程
void EventLoop::wakeUp(){
    uint64_t one = 1;
    ssize_t n = write(wakeUpFd_, &one, sizeof(one));
    if(n != sizeof(one)){
        LOG_ERROR("EventLoop::wakeUp() write %lu bytes instead 0f 8", n);
    }
}
// EventLoop  调用 ==> Poller 中的updateChannel 和removeChannel
void EventLoop::updateChannel(Channel* channel){
    poller_->updateChannel(channel);
}
void EventLoop::removeChannel(Channel* channel){
    poller_->removeChannel(channel);
}
bool EventLoop::hasChannel(Channel* channel){
    return poller_->hasChannel(channel);
}

void EventLoop::doPendingFunctors(){
    std::vector<Func> functors;
    callingPendingFunctors_ = true;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pedingFunctors_);
    }
    for( const Func &functor : functors){
        functor();
    }
    callingPendingFunctors_ = false;
}

}
