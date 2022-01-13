/**
 * @file Channel.cpp
 * @author 冯岳松 (yuesong-feng@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2022-01-04
 *
 * @copyright Copyright (冯岳松) 2022
 *
 */
#include "Channel.h"

#include <unistd.h>

#include <utility>

#include "EventLoop.h"
#include "Socket.h"

const int Channel::READ_EVENT = 1;
const int Channel::WRITE_EVENT = 2;
const int Channel::ET = 4;

Channel::Channel(EventLoop *loop, Socket *socket) : loop_(loop), socket_(socket) {}

Channel::~Channel() { loop_->DeleteChannel(this); }

void Channel::HandleEvent() {
  if (ready_events_ & READ_EVENT) {
    read_callback_();
  }
  if (ready_events_ & WRITE_EVENT) {
    write_callback_();
  }
}

void Channel::EnableRead() {
  listen_events_ |= READ_EVENT;
  loop_->UpdateChannel(this);
}

void Channel::EnableWrite() {
  listen_events_ |= WRITE_EVENT;
  loop_->UpdateChannel(this);
}

void Channel::UseET() {
  listen_events_ |= ET;
  loop_->UpdateChannel(this);
}
Socket *Channel::GetSocket() { return socket_; }

int Channel::GetListenEvents() { return listen_events_; }
int Channel::GetReadyEvents() { return ready_events_; }

bool Channel::GetExist() { return exist_; }

void Channel::SetExist(bool in) { exist_ = in; }

void Channel::SetReadyEvents(int ev) {
  if (ev & READ_EVENT) {
    ready_events_ |= READ_EVENT;
  }
  if (ev & WRITE_EVENT) {
    ready_events_ |= WRITE_EVENT;
  }
  if (ev & ET) {
    ready_events_ |= ET;
  }
}

void Channel::SetReadCallback(std::function<void()> const &callback) { read_callback_ = callback; }
void Channel::SetWriteCallback(std::function<void()> const &callback) { write_callback_ = callback; }
