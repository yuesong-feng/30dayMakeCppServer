/**
 * @file Poller.h
 * @author 冯岳松 (yuesong-feng@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2022-01-04
 *
 * @copyright Copyright (冯岳松) 2022
 *
 */
#pragma once
#include "common.h"
#include <vector>

#ifdef OS_LINUX
#include <sys/epoll.h>
#endif

#ifdef OS_MACOS
#include <sys/event.h>
#endif

class Poller {
 public:
  DISALLOW_COPY_AND_MOVE(Poller);
  Poller();
  ~Poller();

  RC UpdateChannel(Channel *ch) const;
  RC DeleteChannel(Channel *ch) const;

  std::vector<Channel *> Poll(long timeout = -1) const;

 private:
  int fd_;

#ifdef OS_LINUX
  struct epoll_event *events_{nullptr};
#endif

#ifdef OS_MACOS
  struct kevent *events_;
#endif
};
