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
    void setnonblocking();

    int accept(InetAddress*);

    int getFd();
};

