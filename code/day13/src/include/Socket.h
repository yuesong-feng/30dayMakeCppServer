/******************************
 *   author: yuesong-feng
 *
 *
 *
 ******************************/

#pragma once
#include <arpa/inet.h>
#include "Macros.h"

class InetAddress {
 public:
  InetAddress();
  InetAddress(const char *ip, uint16_t port);
  ~InetAddress() = default;

  DISALLOW_COPY(InetAddress);

  void SetAddr(sockaddr_in addr);
  sockaddr_in GetAddr();
  const char *GetIp();
  uint16_t GetPort();

 private:
  struct sockaddr_in addr_;
};

class Socket {
 private:
  int fd_;

 public:
  Socket();
  explicit Socket(int _fd);
  ~Socket();

  DISALLOW_COPY(Socket);

  void Bind(InetAddress *);
  void Listen();
  int Accept(InetAddress *);

  void Connect(InetAddress *);

  void SetNonBlocking();
  int GetFd();
};
