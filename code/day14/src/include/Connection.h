/**
 * @file Connection.h
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
class Buffer;
class Connection {
 public:
  Connection(EventLoop *loop, Socket *sock);
  ~Connection();
  DISALLOW_COPY_AND_MOVE(Connection);

  void Recv();
  void SetDeleteConnectionCallback(std::function<void(int)> const &callback);
  void SetOnConnectCallback(std::function<void(Connection *)> const &callback);
  void Send();

  void OnConnect(std::function<void()> fn);
  Buffer *read_buffer_{nullptr};
  Buffer *send_buffer_{nullptr};

 private:
  EventLoop *loop_;
  Socket *sock_;
  Channel *channel_{nullptr};
  std::function<void(int)> delete_connectioin_callback_;

  std::function<void(Connection *)> on_connect_callback_;
};
