/******************************
 *   author: yuesong-feng
 *
 *
 *
 ******************************/
#include "include/EventLoop.h"

#include <vector>

#include "include/Channel.h"
#include "include/Epoll.h"

EventLoop::EventLoop() : epoll_(nullptr), quit_(false) { epoll_ = new Epoll(); }

EventLoop::~EventLoop() { delete epoll_; }

void EventLoop::Loop() {
  while (!quit_) {
    std::vector<Channel *> chs;
    chs = epoll_->Poll();
    for (auto &ch : chs) {
      ch->HandleEvent();
    }
  }
}

void EventLoop::UpdateChannel(Channel *ch) { epoll_->UpdateChannel(ch); }
