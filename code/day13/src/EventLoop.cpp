/******************************
 *   author: yuesong-feng
 *
 *
 *
 ******************************/
#include "EventLoop.h"

#include <vector>

#include "Channel.h"
#include "Epoll.h"

EventLoop::EventLoop() : ep(nullptr), quit(false) { ep = new Epoll(); }

EventLoop::~EventLoop() { delete ep; }

void EventLoop::loop() {
  while (!quit) {
    std::vector<Channel *> chs;
    chs = ep->poll();
    for (auto &ch : chs) {
      ch->handleEvent();
    }
  }
}

void EventLoop::updateChannel(Channel *ch) { ep->updateChannel(ch); }
