#pragma once

#include "Acceptor.h"
#include "CallBack.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "InetAddress.h"
#include "noncopyable.h"
#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
namespace fst {

class TcpServer : nonccopyable
{
public:
    using ThreadInitCallBack = std::function<void(EventLoop*)>;

    enum Option{
        kNonReusePort,
        kReusePort
    };
    /**
       acceptor 创建新连接 
     */
    TcpServer(EventLoop *loop,
               const InetAddress &listenAddr,
               const std::string &nameArg,
                Option option = kNonReusePort);
    ~TcpServer();
                                                            //notify              
    //打包给TcpServer ==> TcpConnection ==> channel ==> poller ====> channel调用 ===> loop
    void setThreadInitCallBack(const ThreadInitCallBack& cb) { threadInitCallBack_ = cb; }  //可以不设置
    void setMessageCallBack(const MessageCallBack&cb) { messageCallBack_ = cb; }
    void setConnectionCallBack(const ConnectionCallBack&cb) { connectionCallBack_ = cb; }
    void setWriteCallBack(const WriteCompleteCallBack&cb) { writeCompleteCallBack_ = cb; }

    void setThreadNum(int numThreads);
    void start();

private:
    /**
        功能:创建一个新连接 sockfd <== client_fd peerAddr <== clientAddr
        1. 选择一个loop也就是一个线程 
        2. 将client_fd 和 cb 打包成一个TcpConnection对象 添加到 ConnectionMap
        3. 直接调用TcpConnection::connectEstablished （将client_fd添加到epoll）
     */
    void newConnection(int sockfd, const InetAddress &peerAddr);
    /**
        功能：端开一个连接
        1. 将client_fd对应的TcpConnection对象 从ConnectionMap中删除
        2. 直接调用TcpConnection::connectDestroyed (将client_fd从epoll中删除)
     */
    void removeConnection(const TcpConnectionPtr &conn);
    void removeConnectionInLoop(const TcpConnectionPtr &conn);
private:

    using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;

    EventLoop *loop_;
    const std::string ipPort_;
    std::string name_;
    std::unique_ptr<Acceptor> acceptor_;                //accept
    std::shared_ptr<EventLoopThreadPool> threadPool_;

    ConnectionCallBack connectionCallBack_;
    MessageCallBack messageCallBack_;
    WriteCompleteCallBack writeCompleteCallBack_;
    ThreadInitCallBack threadInitCallBack_;

    std::atomic_int started_;
    int nextConnId_;

    ConnectionMap connections_;                         //client_fd
};

}