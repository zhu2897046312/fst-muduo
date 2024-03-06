#pragma once

#include <cstdint>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
namespace fst {

class InetAddress
{
public:
    explicit InetAddress(uint16_t port, std::string ip = "127.0.0.1");
    explicit InetAddress(const sockaddr_in &addr);
    
    std::string toIp() const;
    std::string toIpPort() const;
    uint16_t toPort() const;

    const sockaddr_in* getSockAddr() const;
private:
    sockaddr_in addr_;
};

}