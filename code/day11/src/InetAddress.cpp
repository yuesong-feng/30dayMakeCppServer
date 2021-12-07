/******************************
*   author: yuesong-feng
*   
*
*
******************************/
#include "InetAddress.h"
#include <string.h>

InetAddress::InetAddress() {
    bzero(&addr, sizeof(addr));
}
InetAddress::InetAddress(const char* _ip, uint16_t _port) {
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(_ip);
    addr.sin_port = htons(_port);
}

InetAddress::~InetAddress(){
}

void InetAddress::setInetAddr(sockaddr_in _addr){
    addr = _addr;
}

sockaddr_in InetAddress::getAddr(){
    return addr;
}
