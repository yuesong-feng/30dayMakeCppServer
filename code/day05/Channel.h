#pragma once
#include <sys/epoll.h>

class Epoll;
class Channel
{
private:
    Epoll *ep;
    int fd;
    uint32_t events;
    uint32_t revents;
    bool inEpoll;
public:
    Channel(Epoll *_ep, int _fd);
    ~Channel();

    void enableReading();

    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();
    bool getInEpoll();
    void setInEpoll();

    // void setEvents(uint32_t);
    void setRevents(uint32_t);
};

