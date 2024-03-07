#include "EPollPoller.h"

#include "Channel.h"
#include "Logger.h"
#include "Poller.h"
#include "Timestamp.h"

#include <cerrno>
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

//监听fd上的事件
Timestamp EPollPoller::poll(int timeoutMs, ChannelList* activeChannels){
    LOG_INFO("func=%s => fd total count:%lu\n"
        , __FUNCTION__
        ,channels_.size());

    int numEvents = ::epoll_wait(epoll_fd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
    int saveErrno = errno;
    Timestamp now(Timestamp::now());
    if(numEvents > 0){
        LOG_INFO("%d events hanppened \n",numEvents);
        fillActiveChannels(numEvents, activeChannels);
        if(numEvents == events_.size()){
            events_.resize(events_.size() * 2);
        }
    }else if(numEvents == 0){
        LOG_INFO("%s timeout! \n", __FUNCTION__);
    }else{
        if(saveErrno != EINTR){
            errno = saveErrno;
            LOG_ERROR("EPollPoller::poll() err!");
        }
    }
    return now;
}
void EPollPoller::updateChannel(Channel*channel){
    
    const int index = channel->index();
    LOG_INFO("func=%s fd=%d events=%d index=%d\n"
        , __FUNCTION__
        ,channel->fd()
        ,channel->events()
        ,channel->index());

    if(kNew == index  || kDeleted == index){
        if(kNew == index){
            int fd = channel->fd();
            channels_[fd] = channel;
        }

        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel); //给channel中的fd 添加监听事件
    }else{
        int fd = channel->fd();
        if(channel->isNoneEvent()){
            update(EPOLL_CTL_DEL, channel); //给channel中的fd 删除监听事件
            channel->set_index(kDeleted);
        }else{
            update(EPOLL_CTL_MOD, channel); //给channel中的fd 修改监听事件
        }
    }
}
void EPollPoller::removeChannel(Channel* channel){
    int fd = channel->fd();
    channels_.erase(fd);

    LOG_INFO("func=%s fd=%d events=%d index=%d\n"
        , __FUNCTION__
        ,channel->fd()
        ,channel->events()
        ,channel->index());

    int index = channel->index();
    if(kAdded == index || kDeleted == index){
        update(EPOLL_CTL_DEL, channel);
    }

    channel->set_index(kNew);
}
//填写活跃的连接
void EPollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const{
    for(int i = 0; i < numEvents; i++){
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

//更新Channel通道 fd注册事件
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