#pragma once

//#include "CallBack.h"
#include "Buffer.h"
#include "Channel.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Timestamp.h"
#include "noncopyable.h"

#include <atomic>
#include <cstddef>
#include <memory>
#include <string>
namespace fst {

class TcpConnection : nonccopyable , public std::enable_shared_from_this<TcpConnection>
{
public:

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallBack= std::function<void(const TcpConnectionPtr&)>;
using CloseCallBack = std::function<void(const TcpConnectionPtr&)>;
using WriteCompleteCallBack = std::function<void (const TcpConnectionPtr&)>;

using MessageCallBack = std::function<void(const  TcpConnectionPtr&
                        , Buffer*
                        , Timestamp)>;
using HightWaterMarkCallBack = std::function<void(const TcpConnectionPtr&,size_t)>;

    enum StateE{
        kDisconnected,
        kConnected,
        kConnecting,
        KDisConnecting,
    };
    /**
        1. 将client_fd 打包成channel 添加到epoll上
        2. 设置回调处理客户端请求,如:给客户端发送数据或接收客户端发来的数据
        3. 关闭连接
    */
    TcpConnection(EventLoop *loop
                , const std::string &nameArg
                , int sockfd
                , const InetAddress &localAddr
                , const InetAddress & peerAddr);
    ~TcpConnection();

    EventLoop* getLoop() const { return loop_; }
    const std::string& name() const { return name_; }
    const InetAddress& localAddress() const { return localAddr_; }
    const InetAddress& peerAddress() const { return peerAddr_; }

    bool connected() const { return state_ == kConnected; }
    //给客户端发数据
    void send(std::string &buf);
    //server端主动端开连接 //关闭client_fd
    void shutdown();
    //clinet_fd 注册到epoll上            
    void connectEstablishend();
    //clinet_fd 从epoll删除
    void connectDestroyed();
    
    //处理客户端请求的回调 --- 用户自定义 onMessage() conConnection() 再把这些回调打包给channel
    void setConectionCallBack(const ConnectionCallBack &cb) { connectionCallBack_ = std::move(cb); }
    void setMessageCallBack(const MessageCallBack &cb) { messageCallBack_ = std::move(cb); }
    void setWriteCallBack(const WriteCompleteCallBack &cb) { writeCompleteCallback_ = std::move(cb); }
    void setHightWaterMarkCallBack(const HightWaterMarkCallBack &cb) { hightWaterMarkCallBack_ = std::move(cb); }
    void setCloseCallBack(const CloseCallBack &cb) { closeCallBack_ = cb; }

    void setState(StateE state) { state_.store(static_cast<std::atomic_int>(state));}
private:
     //给客户端发数据
    void sendInLoop(const void *message, size_t len);
    //关闭client_fd
    void shutdownInLoop();

    //处理客户端请求
    void handleRead(Timestamp receliveTime);
    void handleWrite();
    void handleClose();
    void handleError();
private:
    EventLoop *loop_;

    std::string name_;
    std::atomic_int state_;
    bool reading_;

    std::unique_ptr<Socket> socket_; //client_fd
    std::unique_ptr<Channel> channel_;

    const InetAddress localAddr_;
    const InetAddress peerAddr_;

    ConnectionCallBack connectionCallBack_;
    MessageCallBack messageCallBack_;
    WriteCompleteCallBack writeCompleteCallback_;
    HightWaterMarkCallBack hightWaterMarkCallBack_;
    CloseCallBack closeCallBack_;
    size_t hightWaterMark_;

    Buffer inputBuffer_;    //读缓冲区 读取client_fd客户端发来的数据
    Buffer outputBuffer_;   //写缓冲区 向client_fd写数据发送给客户端
};

}