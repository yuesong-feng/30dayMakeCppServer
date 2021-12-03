#pragma once
#include <sys/epoll.h>
#include <functional>
class EventLoop;
class Channel
{
private:
    EventLoop *loop;
    int fd;
    uint32_t events;
    uint32_t revents;
    bool inEpoll;
    std::function<void()> callback;
public:
    Channel(EventLoop *_loop, int _fd);
    ~Channel();

    void handleEvent();
    void enableReading();

    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();
    bool getInEpoll();
    void setInEpoll();

    // void setEvents(uint32_t);
    void setRevents(uint32_t);
    void setCallback(std::function<void()>);
};

