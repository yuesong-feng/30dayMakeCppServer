/******************************
 *   author: yuesong-feng
 *
 *
 *
 ******************************/
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
  DISALLOW_COPY(Connection);

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
