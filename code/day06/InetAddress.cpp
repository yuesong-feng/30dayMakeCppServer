#include "InetAddress.h"
#include <string.h>
InetAddress::InetAddress() : addr_len(sizeof(addr)){
    bzero(&addr, sizeof(addr));
}
InetAddress::InetAddress(const char* ip, uint16_t port) : addr_len(sizeof(addr)){
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    addr_len = sizeof(addr);
}

InetAddress::~InetAddress(){
}
