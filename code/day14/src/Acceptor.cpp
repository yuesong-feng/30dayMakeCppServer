/**
 * @file Acceptor.cpp
 * @author 冯岳松 (yuesong-feng@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2022-01-04
 *
 * @copyright Copyright (冯岳松) 2022
 *
 */
#include "Acceptor.h"

#include <utility>

#include "Channel.h"

#include "Socket.h"

Acceptor::Acceptor(EventLoop *loop) : loop_(loop), sock_(nullptr), channel_(nullptr) {
  sock_ = new Socket();
  InetAddress *addr = new InetAddress("127.0.0.1", 1234);
  sock_->Bind(addr);
  // sock->setnonblocking(); acceptor使用阻塞式IO比较好
  sock_->Listen();
  channel_ = new Channel(loop_, sock_->GetFd());
  std::function<void()> cb = std::bind(&Acceptor::AcceptConnection, this);
  channel_->SetReadCallback(cb);
  channel_->EnableRead();
  delete addr;
}

Acceptor::~Acceptor() {
  delete channel_;
  delete sock_;
}

void Acceptor::AcceptConnection() {
  InetAddress *clnt_addr = new InetAddress();
  Socket *clnt_sock = new Socket(sock_->Accept(clnt_addr));
  printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->GetFd(), clnt_addr->GetIp(), clnt_addr->GetPort());
  clnt_sock->SetNonBlocking();  // 新接受到的连接设置为非阻塞式
  new_connection_callback_(clnt_sock);
  delete clnt_addr;
}

void Acceptor::SetNewConnectionCallback(std::function<void(Socket *)> const &callback) {
  new_connection_callback_ = callback;
}
