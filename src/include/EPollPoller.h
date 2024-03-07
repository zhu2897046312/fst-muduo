#pragma once

#include "Channel.h"
#include "EventLoop.h"
#include "Poller.h"
#include <sys/epoll.h>
#include <vector>
namespace fst {

/**
 * epoll_create
 * epoll_ctl
 * epoll_wait
 */
class EPollPoller : public Poller
{
public:
    EPollPoller(EventLoop *loop);
    virtual ~EPollPoller() override;
    //设置监听fd上的事件
    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) override;
    //修改fd注册的事件
    virtual void updateChannel(Channel*channel) override;
    //移除fd注册的事件
    virtual void removeChannel(Channel* channel) override;
private:
    //填写活跃的连接
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

    //更新Channel通道
    void update(int operation, Channel*channel);
private:
    using EventList = std::vector<epoll_event>;

    int epoll_fd_;      // eopll 句柄
    EventList events_;  //就绪事件

    static const int kInitEventListSize = 16;
};

}