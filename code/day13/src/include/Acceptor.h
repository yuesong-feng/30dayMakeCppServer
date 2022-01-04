/**
 * @file Acceptor.h
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

class EventLoop;
class Socket;
class Channel;
class Acceptor {
 public:
  explicit Acceptor(EventLoop *loop);
  ~Acceptor();

  DISALLOW_COPY_AND_MOVE(Acceptor);

  void AcceptConnection();
  void SetNewConnectionCallback(std::function<void(Socket *)> const &callback);

 private:
  EventLoop *loop_;
  Socket *sock_;
  Channel *channel_;
  std::function<void(Socket *)> new_connection_callback_;
};
