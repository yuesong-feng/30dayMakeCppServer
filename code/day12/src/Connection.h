/******************************
*   author: yuesong-feng
*   
*
*
******************************/
#pragma once
#include <functional>
class EventLoop;
class Socket;
class Channel;
class Buffer;
class Connection
{
private:
    EventLoop *loop;
    Socket *sock;
    Channel *channel;
    std::function<void(int)> deleteConnectionCallback;
    Buffer *readBuffer;
public:
    Connection(EventLoop *_loop, Socket *_sock);
    ~Connection();
    
    void echo(int sockfd);
    void setDeleteConnectionCallback(std::function<void(int)>);
    void send(int sockfd);
};

