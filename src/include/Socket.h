#pragma once

#include "InetAddress.h"
#include "noncopyable.h"
#include <unistd.h>
namespace fst {


/**
    Socket封装
        1. socket_fd local_address server_address
        2. bind
        3. listen
        4. accept
 */
class Socket  : nonccopyable
{
public:
    explicit Socket(const int sockfd)
        : sockfd_(sockfd){}

    ~Socket() { close(sockfd_); }

    int fd() const { return sockfd_; }

    void bindAddress(const InetAddress &localaddr);
    void listen();
    int accept(InetAddress *peeraddr);

    void shutdownWrite();

    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);
private:
    const int sockfd_;
};

}