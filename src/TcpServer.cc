#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "Logger.h"
#include <functional>
#include <string>

namespace fst {

 static EventLoop* checkLoopNotNull(EventLoop*loop){
    if(loop == nullptr){
        LOG_FATAL("%s:%s:%d mainloop is null\n"
        , __FILE__
        , __FUNCTION__
        , __LINE__);
    }
    return loop;
}

/**
    1.监听指定端口8000,接收客户端请求
    2.管理客户端连接(连接建立、断开)
    3.提供回调接口(处理不同事件,如新连接到来、数据到达)
    4.处理TCP连接的读写事件,保证数据能够正确地发送和接收。
*/
TcpServer::TcpServer(EventLoop *loop,
                const InetAddress &listenAddr,
                const std::string &nameArg,
                Option option)
                : loop_(checkLoopNotNull(loop))
                , ipPort_(listenAddr.toIpPort())
                , name_(nameArg)
                , acceptor_(new Acceptor(loop,listenAddr,option == kReusePort))
                , threadPool_(new EventLoopThreadPool(loop,name_))
                , connectionCallBack_()
                , messageCallBack_()
                , nextConnId_(1)
                , started_(0){
    acceptor_->setNewConnectionCallBack(
        std::bind(&TcpServer::newConnection 
                    , this
                    , std::placeholders::_1
                    , std::placeholders::_2));
}

TcpServer::~TcpServer(){
}

void TcpServer::setThreadNum(int numThreads){
    threadPool_->setThreadNum(numThreads);
}

void TcpServer::start(){
    if(started_++ == 0){
        threadPool_->start(threadInitCallBack_);
        loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
    }
}

//创建新连接
void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr){
// 轮询算法，选择一个subLoop，来管理channel
    EventLoop *ioLoop = threadPool_->getNextLoop(); 
    char buf[64] = {0};
    snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;

    LOG_INFO("TcpServer::newConnection [%s] - new connection [%s] from %s \n",
        name_.c_str(), connName.c_str(), peerAddr.toIpPort().c_str());

    // 通过sockfd获取其绑定的本机的ip地址和端口信息
    sockaddr_in local;
    //::bzero(&local, sizeof local);
    socklen_t addrlen = sizeof local;
    if (::getsockname(sockfd, (sockaddr*)&local, &addrlen) < 0)
    {
        LOG_ERROR("sockets::getLocalAddr");
    }
    InetAddress localAddr(local);

    // 根据连接成功的sockfd，创建TcpConnection连接对象
    TcpConnectionPtr conn(new TcpConnection(
                            ioLoop,
                            connName,
                            sockfd,   // client_fd
                            localAddr,
                            peerAddr));
    connections_[connName] = conn;
    // 下面的回调都是用户设置给TcpServer=>TcpConnection=>Channel=>Poller=>notify channel调用回调
    conn->setConectionCallBack(connectionCallBack_);
    conn->setMessageCallBack(messageCallBack_);         //连接成功要干什么   == 用户自定
    conn->setWriteCallBack(writeCompleteCallBack_);
    // 设置了如何关闭连接的回调   conn->shutDown()
    conn->setCloseCallBack(
        std::bind(&TcpServer::removeConnection, this, std::placeholders::_1)
    );

    // 直接调用TcpConnection::connectEstablished
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablishend, conn));
}
void TcpServer::removeConnection(const TcpConnectionPtr &conn)
{
    loop_->runInLoop(
        std::bind(&TcpServer::removeConnectionInLoop, this, conn)
    );
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn)
{
    LOG_INFO("TcpServer::removeConnectionInLoop [%s] - connection %s\n", 
        name_.c_str(), conn->name().c_str());

    connections_.erase(conn->name());
    EventLoop *ioLoop = conn->getLoop(); 
    ioLoop->queueInLoop(
        std::bind(&TcpConnection::connectDestroyed, conn)
    );
}

}