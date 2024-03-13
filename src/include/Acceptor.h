#pragma once

#include "Channel.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"
#include "noncopyable.h"

#include <functional>
#include <sys/eventfd.h>
namespace fst {

class Acceptor : nonccopyable
{
public:
    using NewConnectionCallBack = std::function<void(int sockfd, const InetAddress&)>;
    Acceptor(EventLoop *loop, const InetAddress &server_Addr, bool reuseport);
    ~Acceptor();

    bool listening() { return listenning_; }
    //监听 server_fd 
    void listen();

    void setNewConnectionCallBack(const NewConnectionCallBack &cb){ newConnectionCallBack_ = std::move(cb); }
private:
    /**
        1. server_fd有读事件发生(即有客户端连接请求) 
        2. epoll_wait()出来 
        3. client_fd=accept() 接收客户端请求
        4. 连接成功 -- (处理客户端请求)处理客户端发送的数据或给客户端发送数据
     */
    void handleRead();     

private:

    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    
    bool listenning_;
    NewConnectionCallBack newConnectionCallBack_;   //来了一条新连接 ===> 要干什么？？？ 执行这个回调操作即是要干的事 
};

}