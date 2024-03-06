#include "Poller.h"

namespace fst {

Poller::Poller(EventLoop *loop)
    : ownerLoop_(loop){
}

//判断参数Channel是否在当前Poller中
bool Poller::hasChannel(Channel* channel) const{
    auto it = channels_.find(channel->fd());
    return it != channels_.end() && it->second == channel;
}


}