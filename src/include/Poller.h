#pragma once

#include "Channel.h"
#include "EventLoop.h"
#include "Timestamp.h"
#include "noncopyable.h"
#include <unordered_map>
#include <vector>
namespace fst {

class Channel;
class EventLoop;

// muduo库朱红多路事件分发器的核心IO复用
class Poller : nonccopyable
{
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop *loop);
    virtual ~Poller() = default;

    //给所有IO复用保留统一的接口
    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;
    virtual void updateChannel(Channel*channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;

    //判断参数Channel是否在当前Poller中
    bool hasChannel(Channel* channel) const;

    //EentLoop可以通过该接口获取默认的IO复用的具体实现
    static Poller* newDefaltPoller(EventLoop *loop);
protected:
    //map的key : sockfd  value : sockfd所属的Channel通道类型
    using ChannelMap = std::unordered_map<int, Channel*>;
    ChannelMap channels_;
private:
    EventLoop* ownerLoop_;
};

}