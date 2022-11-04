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
#include <functional>
#include "common.h"

class Connection {
 public:
  enum State {
    Invalid = 0,
    Connecting,
    Connected,
    Closed,
  };
  DISALLOW_COPY_AND_MOVE(Connection);
  Connection(int fd, EventLoop *loop);
  ~Connection();

  void set_delete_connection(std::function<void(int)> const &fn);
  void set_on_connect(std::function<void(Connection *)> const &fn);
  void set_on_recv(std::function<void(Connection *)> const &fn);
  State state() const;
  Socket *socket() const;
  void set_send_buf(const char *str);
  Buffer *read_buf();
  Buffer *send_buf();

  RC Read();
  RC Write();
  RC Send(std::string msg);

  void Close();

  void onConnect(std::function<void()> fn);
  void onMessage(std::function<void()> fn);

 private:
  void Business();
  RC ReadNonBlocking();
  RC WriteNonBlocking();
  RC ReadBlocking();
  RC WriteBlocking();

 private:
  std::unique_ptr<Socket> socket_;
  std::unique_ptr<Channel> channel_;

  State state_;
  std::unique_ptr<Buffer> read_buf_;
  std::unique_ptr<Buffer> send_buf_;

  std::function<void(int)> delete_connectioin_;
  std::function<void(Connection *)> on_recv_;
};
