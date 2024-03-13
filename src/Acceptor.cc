#include "Acceptor.h"
#include "Logger.h"
#include <cerrno>
#include <cstdlib>
#include <netinet/in.h>
#include <sys/socket.h>

namespace fst {
//create socket_fd Nonblocking
static int createNonblocking(){
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,IPPROTO_TCP);
    if(sockfd < 0){
        LOG_FATAL("%s:%s:%d listen socket create err:%d  \n"
            , __FILE__ 
            , __FUNCTION__
            , __LINE__
            ,errno);
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

Acceptor::Acceptor(EventLoop *loop, const InetAddress &server_Addr, bool reuseport)
    : loop_(loop)
    , acceptSocket_(createNonblocking())
    , acceptChannel_(loop, acceptSocket_.fd())
    , listenning_(false){
        acceptSocket_.setReuseAddr(true);
        acceptSocket_.setReusePort(true);
        acceptSocket_.bindAddress(server_Addr);
        
        acceptChannel_.setReadCallBack(std::bind(&Acceptor::handleRead, this));     //设置可读操作回调

}

Acceptor::~Acceptor(){
    acceptChannel_.disableAll();
    acceptChannel_.remove();
}

void Acceptor::listen(){    
    listenning_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading(); //设置可读 
}
void Acceptor::handleRead(){
    InetAddress peerAddr;
    int connfd = acceptSocket_.accept(&peerAddr);   //没有连接时阻塞    -- 有连接时 就绪 ==> 执行相应操作  
    if(connfd >= 0){
        /** 
         创建一个新连接 connfd = client_fd peerAddr = clientAddr
         1. client_fd 添加到 epoll 中监听
        */
        if(newConnectionCallBack_){
            newConnectionCallBack_(connfd, peerAddr); 
        }else{
            ::close(connfd);
        }
    }else{
        LOG_ERROR("%s:%s:%d accept err:%d  \n"
            , __FILE__ 
            , __FUNCTION__
            , __LINE__
            ,errno);
        if(errno == EMFILE){
            LOG_ERROR("%s:%s:%d sockfd reachhed limit err  \n"
            , __FILE__ 
            , __FUNCTION__
            , __LINE__);
        }
    }
}

}