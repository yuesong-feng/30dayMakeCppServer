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

  void Echo(int sockfd);
  void SetDeleteConnectionCallback(std::function<void(int)> const &callback);
  void Send(int sockfd);

 private:
  EventLoop *loop_;
  Socket *sock_;
  Channel *channel_;
  std::function<void(int)> delete_connectioin_callback_;
  Buffer *read_buffer_;
};
