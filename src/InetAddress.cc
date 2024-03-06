#include "InetAddress.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <strings.h>
#include <arpa/inet.h>
#include <string.h>

namespace fst {

InetAddress::InetAddress(uint16_t port, std::string ip){
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
}

InetAddress::InetAddress(const sockaddr_in &addr) 
        : addr_(addr){}
    
std::string InetAddress::toIp() const{
    char buf[64] = {0};
    ::inet_ntop(AF_INET, &addr_, buf, sizeof(buf));
    return std::string(buf);
}

std::string InetAddress::toIpPort() const{
    char buf[64] = {0};
    ::inet_ntop(AF_INET, &addr_, buf, sizeof(buf));
    size_t end = strlen(buf);
    uint16_t port = ntohs(addr_.sin_port);
    sprintf(buf + end, ":%u",port);
    return std::string(buf);
}

uint16_t InetAddress::toPort() const{
    return ntohs(addr_.sin_port);
}

const sockaddr_in* InetAddress::getSockAddr() const {
    return &addr_;
}

}