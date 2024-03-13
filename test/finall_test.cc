#include "TcpServer.h"
#include "Logger.h"
#include "EventLoop.h"

#include <string>
#include <functional>

class EchoServer
{
public:
    EchoServer(fst::EventLoop *loop,
            const fst::InetAddress &addr, 
            const std::string &name)
        : loop_(loop), 
        server_(loop_, addr, name)
    {
        // 注册回调函数
        server_.setConnectionCallBack(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));

        server_.setMessageCallBack(
            std::bind(&EchoServer::onMessage, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
        );

        // 设置合适的loop线程数量 loopthread
        server_.setThreadNum(3);
    }
    void start()
    {
        server_.start();
    }
private:
    // 连接建立或者断开的回调
    void onConnection(const fst::TcpConnectionPtr &conn)
    {
        if (conn->connected())
        {
            LOG_INFO("Connection UP : %s", conn->peerAddress().toIpPort().c_str());
            LOG_INFO("=======================================");
            LOG_INFO("%s:%s:%d",__FILE__,__FUNCTION__,__LINE__);
            LOG_INFO("=======================================");
        }
        else
        {
            LOG_INFO("Connection DOWN : %s", conn->peerAddress().toIpPort().c_str());
        }
    }

    // 可读写事件回调
    void onMessage(const fst::TcpConnectionPtr &conn,
                fst::Buffer *buf,
                fst::Timestamp time)
    {
        std::string msg = buf->retrieveAllAsString();
        conn->send(msg);
        conn->shutdown(); // 写端   EPOLLHUP =》 closeCallback_
    }

    fst::EventLoop *loop_;
    fst::TcpServer server_;
};

int main()
{
    fst::EventLoop loop;
    fst::InetAddress addr(8000);
    EchoServer server(&loop, addr, "EchoServer-01"); // Acceptor non-blocking listenfd  create bind 
    server.start(); // listen  loopthread  listenfd => acceptChannel => mainLoop =>
    loop.loop(); // 启动mainLoop的底层Poller

    return 0;
}