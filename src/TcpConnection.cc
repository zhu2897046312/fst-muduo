#include "TcpConnection.h"
#include "Channel.h"
#include "Logger.h"
#include "Socket.h"
#include <asm-generic/socket.h>
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <memory>
#include <sys/types.h>
#include <unistd.h>

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

TcpConnection::TcpConnection(EventLoop *loop
                , const std::string &nameArg
                , int sockfd
                , const InetAddress &localAddr
                , const InetAddress & peerAddr)
                : loop_(checkLoopNotNull(loop))
                , name_(nameArg)
                , state_(kConnecting)
                , reading_(true)
                , socket_(new Socket(sockfd))   //client_fd
                , channel_(new Channel(loop,sockfd))   // 打包成channel添加到epoll上
                , peerAddr_(peerAddr)
                , localAddr_(localAddr)
                , hightWaterMark_(64*1024*1024){
    channel_->setReadCallBack(
        std::bind(&TcpConnection::handleRead,this,std::placeholders::_1));
    channel_->setWriteCallBack(
        std::bind(&TcpConnection::handleWrite,this));
    channel_->setCloseCallBack(
        std::bind(&TcpConnection::handleClose,this));
    channel_->setErrorCallBack(
        std::bind(&TcpConnection::handleError,this));

    LOG_INFO("TcpConnection::ctor[%s] at fd=%d\n",name_.c_str(),sockfd);
    socket_->setKeepAlive(true);
}
TcpConnection::~TcpConnection(){ 
    LOG_INFO("TcpConnection::dtor[%s] at fd=%d state=%d\n", name_.c_str(), channel_->fd(), static_cast<int>(state_));
}

void TcpConnection::send(std::string &buf){
    if(state_ == kConnected){
        if(loop_->isInLoopThread()){
            sendInLoop(buf.c_str(), buf.size());
        }else{
            loop_->runInLoop(std::bind(&TcpConnection::sendInLoop,this,buf.c_str(),buf.size()));
        }
    }
}
//给客户端发数据
void TcpConnection::sendInLoop(const void *message, size_t len){
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;

    if(state_ == kDisconnected){
        LOG_ERROR("disconnected ,give up writing!\n");
        return;
    }
    if(!channel_->isWriting() && outputBuffer_.readableBytes() == 0){
        nwrote = write(channel_->fd(), message, len);
        if(nwrote >= 0){
            remaining = len - nwrote;
            if(remaining == 0 && writeCompleteCallback_){
                loop_->queueInLoop(std::bind(writeCompleteCallback_,shared_from_this()));
            }
        }
    }else{
        nwrote = 0;
        if(errno != EWOULDBLOCK){
            LOG_ERROR("TcpConnection::sendInLoop error\n");
            if(errno == EPIPE || errno == ECONNRESET){
                faultError = true;
            }
        }
    }

    if(!faultError && remaining > 0){
        size_t oldLen = outputBuffer_.readableBytes();
        if (oldLen + remaining >= hightWaterMark_
            && oldLen < hightWaterMark_
            && hightWaterMarkCallBack_)
        {
            loop_->queueInLoop(
                std::bind(hightWaterMarkCallBack_, shared_from_this(), oldLen+remaining)
            );
        }
        outputBuffer_.append((char*)message + nwrote, remaining);
        if (!channel_->isWriting())
        {
            channel_->enableWriting(); // 这里一定要注册channel的写事件，否则poller不会给channel通知epollout
        }
    }
}

void TcpConnection::shutdown(){
     if (state_ == kConnected)
    {
        setState(KDisConnecting);
        loop_->runInLoop(
            std::bind(&TcpConnection::shutdownInLoop, this)
        );
    }
}
//关闭client_fd
void TcpConnection::shutdownInLoop(){
    if (!channel_->isWriting()) // 说明outputBuffer中的数据已经全部发送完成
    {
        socket_->shutdownWrite(); // 关闭写端
    }
}
//clinet_fd 注册到epoll上
void TcpConnection::connectEstablishend(){
    setState(kConnected);
    channel_->tie(shared_from_this());
    channel_->enableReading(); // 向poller注册channel的epollin事件

    // 新连接建立，执行回调
    connectionCallBack_(shared_from_this());
}
//clinet_fd 从epoll删除
void TcpConnection::connectDestroyed(){
    if (state_ == kConnected)
    {
        setState(kDisconnected);
        channel_->disableAll(); // 把channel的所有感兴趣的事件，从poller中del掉
        connectionCallBack_(shared_from_this());
    }
    channel_->remove(); // 把channel从poller中删除掉
}

//从client_fd客户端读数据   -- 执行数据到达的回调函数
void TcpConnection::handleRead(Timestamp receliveTime){
    int saveErrno = 0;
    size_t n = inputBuffer_.readFd(channel_->fd(), &saveErrno);             //connfd读数据
    if(n > 0){
        messageCallBack_(shared_from_this(),&inputBuffer_,receliveTime);    //执行回调
    }else if( n == 0){
        handleClose();
    }else{
        errno = saveErrno;
        LOG_ERROR("TcpConnection::handleRead");
        handleError();
    }
}
//给客户端发数据
void TcpConnection::handleWrite(){
    int saveErrno = 0;
    if(channel_->isWriting()){
        ssize_t n = outputBuffer_.writeFd(channel_->fd(),&saveErrno);       //connfd写数据
        if(n > 0){
            outputBuffer_.retrieve(n);
            if(outputBuffer_.readableBytes() == 0){
                channel_->disableReading();
                if(writeCompleteCallback_){
                    // channel 又丢给loop做
                    loop_->queueInLoop(
                                std::bind(writeCompleteCallback_,shared_from_this()));
                }
                if(state_ == KDisConnecting){
                    shutdownInLoop();
                }
            }
        }else{
            LOG_ERROR("TcpConnection::handleWrite error\n");
        }
    }else{
        LOG_ERROR("TcpConnection fd=%d is down, no more writing\n",channel_->fd());
    }
}
void TcpConnection::handleClose(){
    //关闭客户端连接
     LOG_INFO("fd=%d state=%d\n",channel_->fd(),static_cast<int>(state_));
     setState(kDisconnected);
     channel_->disableAll();

    TcpConnectionPtr connPtr(shared_from_this());
    connectionCallBack_(connPtr);
    closeCallBack_(connPtr);
}
void TcpConnection::handleError(){
    //错误处理  -- 打印错误日志
    int optval = 0;
    socklen_t optlen = sizeof(optlen);
    int err = 0;
    if(getsockopt(channel_->fd(), SOL_SOCKET,SO_ERROR, &optval, &optlen)){
        err = errno;
    }else{
        err = optval;
    }
    LOG_ERROR("TcpConnection::handleError name:%s - SO_ERROR:%d\n",name_.c_str(), err);
}




}