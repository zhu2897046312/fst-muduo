#include "EPollPoller.h"

#include "Logger.h"
#include "Poller.h"

#include <cstdlib>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>

namespace fst {

// channel 未添加到poller中
const int kNew = -1;
// channel 已经添加到poller中
const int kAdded = 1;
// channel 已经从poller中删除
const int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop *loop)
    : Poller(loop)
    , epoll_fd_(::epoll_create1(EPOLL_CLOEXEC))
    , events_(kInitEventListSize){ 
    if(epoll_fd_ < 0){
        LOG_FATAL("epoll_create error:%d\n", errno);
        exit(EXIT_FAILURE);
    }
}
EPollPoller::~EPollPoller(){
    ::close(epoll_fd_);
}

Timestamp EPollPoller::poll(int timeoutMs, ChannelList* activeChannels){
    
}
void EPollPoller::updateChannel(Channel*channel){
    
    const int index = channel->index();
    LOG_INFO("fd=%d events=%d index=%d\n"
        ,channel->fd()
        ,channel->events()
        ,channel->index());

    if(kNew == index  || kDeleted == index){
        if(kNew == index){
            int fd = channel->fd();
            channels_[fd] = channel;
        }

        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }else{
        int fd = channel->fd();
        if(channel->isNoneEvent()){
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }else{
            update(EPOLL_CTL_MOD, channel);;
        }
    }
}
void EPollPoller::removeChannel(Channel* channel){
    int fd = channel->fd();
    channels_.erase(fd);

    int index = channel->index();
    if(kAdded == index || kDeleted == index){
        update(EPOLL_CTL_DEL, channel);
    }

    channel->set_index(kNew);
}
//填写活跃的连接
void EPollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const{
    
}

//更新Channel通道
void EPollPoller::update(int operation, Channel*channel){
    
    int fd = channel->fd();

    epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = channel->events();
    event.data.fd = fd;
    event.data.ptr = channel;

    if(::epoll_ctl(epoll_fd_, operation, fd, &event) < 0){
        if(operation == EPOLL_CTL_DEL){
            LOG_ERROR("epoll_ctl del error:%d\n", errno);
        }else{
            LOG_ERROR("epoll_ctl add/mod error:%d\n", errno);
        }
    }

}

}