/**
 * @file EventLoop.cpp
 * @author 冯岳松 (yuesong-feng@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2022-01-04
 *
 * @copyright Copyright (冯岳松) 2022
 *
 */
#include "EventLoop.h"

#include <vector>

#include "Channel.h"
#include "Epoll.h"

EventLoop::EventLoop() { epoll_ = new Epoll(); }

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
