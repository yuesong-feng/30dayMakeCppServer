/**
 * @file Channel.h
 * @author 冯岳松 (yuesong-feng@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2022-01-04
 *
 * @copyright Copyright (冯岳松) 2022
 *
 */
#pragma once
#include "Macros.h"

#include <functional>

class Socket;
class EventLoop;
class Channel {
 public:
  Channel(EventLoop *loop, int fd);
  ~Channel();

  DISALLOW_COPY_AND_MOVE(Channel);

  void HandleEvent();
  void EnableRead();

  int GetFd();
  uint32_t GetListenEvents();
  uint32_t GetReadyEvents();
  bool GetInEpoll();
  void SetInEpoll(bool in = true);
  void UseET();

  void SetReadyEvents(uint32_t ev);
  void SetReadCallback(std::function<void()> const &callback);

 private:
  EventLoop *loop_;
  int fd_;
  uint32_t listen_events_;
  uint32_t ready_events_;
  bool in_epoll_;
  std::function<void()> read_callback_;
  std::function<void()> write_callback_;
};
