/**
 * @file Epoll.h
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

#include <vector>

#ifdef OS_LINUX
#include <sys/epoll.h>
#endif

class Channel;
class Epoll {
 public:
  Epoll();
  ~Epoll();

  DISALLOW_COPY_AND_MOVE(Epoll);

  void UpdateChannel(Channel *ch);
  void DeleteChannel(Channel *ch);

  std::vector<Channel *> Poll(int timeout = -1);

 private:
  int epfd_{1};
  struct epoll_event *events_{nullptr};
};
