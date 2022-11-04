/**
 * @file Socket.h
 * @author 冯岳松 (yuesong-feng@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2022-01-04
 *
 * @copyright Copyright (冯岳松) 2022
 *
 */

#pragma once
#include <string>
#include "common.h"

class Socket {
 public:
  DISALLOW_COPY_AND_MOVE(Socket);
  Socket();
  ~Socket();
  void set_fd(int fd);
  int fd() const;
  std::string get_addr() const;
  RC Create();
  RC Bind(const char *ip, uint16_t port) const;
  RC Listen() const;
  RC Accept(int &clnt_fd) const;
  RC Connect(const char *ip, uint16_t port) const;
  RC SetNonBlocking() const;
  bool IsNonBlocking() const;
  size_t RecvBufSize() const;

 private:
  int fd_;
};
