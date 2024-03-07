#include "Channel.h"

#include "Logger.h"

#include <sys/epoll.h>
namespace fst {

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop, int fd)
    :   loop_(loop), 
        fd_(fd),
        events_(0),
        revents_(0),
        index_(-1),
        tied_(false){}

Channel::~Channel(){
}

// fd得到Poller通知后,处理事件
void Channel::handleEvent(Timestamp receiveTime){
    //从弱智能指针提升成强智能指针
    /**
    从弱智能指针（std::weak_ptr）提升为强智能指针（std::shared_ptr）
    可以使用std::weak_ptr的lock()方法实现。lock()方法会返回一个std::shared_ptr对象，
    该对象与原始的std::shared_ptr共享资源所有权。
    */
    if(tied_){
        std::shared_ptr<void> guard = tie_.lock();
        if(guard){
            handleEventWithGuard(receiveTime);
        }
    }else{
        handleEventWithGuard(receiveTime);
    }
}

// 防止Channel被手动remove掉,Channel还在执行回调操作
void Channel::tie(const std::shared_ptr<void>& obj){
    tie_ = obj;
    tied_ = true;
}

//当改变Channel所表示的events事件后,update负责在Poller里面更改fd相应的事件epoll_ctl()
void Channel::update(){
    //t通过Channel所属的EentLoop,调用Poller相应的方法,注册fd的events事件

    //loop_->udateChannel(this);
}

//根据Poller通知的Channel发生的具体事件,由Channel负责调用具体的回调操作
void Channel::handleEventWithGuard(Timestamp receiveTime){
    
    LOG_INFO("Channel handleEvent revents:%d\n", revents_);
    
    //关闭事件
    if((revents_ & EPOLLHUP) && ! (revents_ & EPOLLIN)){
        if(closeCallBack_){
            closeCallBack_();
        }
    }
    //错误事件
    if(revents_ & EPOLLERR){
        if(errorCallBack_){
            errorCallBack_();
        }
    }
    //可读事件
    if(revents_ & (EPOLLIN | EPOLLPRI)){
        if(readCallBack_){
            readCallBack_(receiveTime);
        }
    }
    //可写事件
    if(revents_ & EPOLLOUT){
        if(writeCallBack_){
            writeCallBack_();
        }
    }
}
//在Channel所属的EentLoop中,把当前的Channel删除掉
void Channel::remove(){

    //loop_->remove(this);
}

}