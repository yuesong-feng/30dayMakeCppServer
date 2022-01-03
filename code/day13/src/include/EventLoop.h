/******************************
 *   author: yuesong-feng
 *
 *
 *
 ******************************/
#pragma once
#include "Macros.h"

#include <functional>

class Epoll;
class Channel;
class EventLoop {
 public:
  EventLoop();
  ~EventLoop();

  DISALLOW_COPY(EventLoop);

  void Loop();
  void UpdateChannel(Channel *);

 private:
  Epoll *epoll_;
  bool quit_;
};
