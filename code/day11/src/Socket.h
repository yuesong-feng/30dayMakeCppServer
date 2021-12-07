/******************************
*   author: yuesong-feng
*   
*
*
******************************/

#pragma once

class InetAddress;
class Socket
{
private:
    int fd;
public:
    Socket();
    Socket(int _fd);
    ~Socket();

    void bind(InetAddress*);
    void listen();
    int accept(InetAddress*);

    void connect(InetAddress*);

    void setnonblocking();
    int getFd();
};

