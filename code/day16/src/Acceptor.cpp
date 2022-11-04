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

#include <fcntl.h>
#include <utility>
#include "Channel.h"
#include "Socket.h"

Acceptor::Acceptor(EventLoop *loop) {
  socket_ = std::make_unique<Socket>();
  assert(socket_->Create() == RC_SUCCESS);
  assert(socket_->Bind("127.0.0.1", 1234) == RC_SUCCESS);
  assert(socket_->Listen() == RC_SUCCESS);

  channel_ = std::make_unique<Channel>(socket_->fd(), loop);
  std::function<void()> cb = std::bind(&Acceptor::AcceptConnection, this);

  channel_->set_read_callback(cb);
  channel_->EnableRead();
}

Acceptor::~Acceptor() {}

RC Acceptor::AcceptConnection() const{
  int clnt_fd = -1;
  if( socket_->Accept(clnt_fd) != RC_SUCCESS ) {
    return RC_ACCEPTOR_ERROR;
  }
  // TODO: remove
  fcntl(clnt_fd, F_SETFL, fcntl(clnt_fd, F_GETFL) | O_NONBLOCK);  // 新接受到的连接设置为非阻塞式
  if (new_connection_callback_) {
    new_connection_callback_(clnt_fd);
  }
  return RC_SUCCESS;
}

void Acceptor::set_new_connection_callback(std::function<void(int)> const &callback) {
  new_connection_callback_ = std::move(callback);
}
