/******************************
 *   author: yuesong-feng
 *
 *
 *
 ******************************/
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

  DISALLOW_COPY(Epoll);

  void UpdateChannel(Channel *ch);
  void DeleteChannel(Channel *ch);

  std::vector<Channel *> Poll(int timeout = -1);

 private:
  int epfd_{1};
  struct epoll_event *events_{nullptr};
};
