#include "InetAddress.h"

#include <iostream>

int main(){
    fst::InetAddress addr(8000);

    std::cout << "toIp : " << addr.toIp() << std::endl;
    std::cout << "toIpPort : " << addr.toIpPort() << std::endl;
    std::cout << "toPort : " << addr.toPort() << std::endl;

    return 0;
}