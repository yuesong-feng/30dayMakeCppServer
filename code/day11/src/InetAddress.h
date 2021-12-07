/******************************
*   author: yuesong-feng
*   
*
*
******************************/
#pragma once
#include <arpa/inet.h>

class InetAddress
{
private:
    struct sockaddr_in addr;
public:
    InetAddress();
    InetAddress(const char* _ip, uint16_t _port);
    ~InetAddress();

    void setInetAddr(sockaddr_in _addr);
    sockaddr_in getAddr();
};

