/**
 * @file Connection.cpp
 * @author 冯岳松 (yuesong-feng@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2022-01-04
 *
 * @copyright Copyright (冯岳松) 2022
 *
 */
#include "Connection.h"

#include <unistd.h>

#include <cstring>
#include <iostream>
#include <utility>

#include "Buffer.h"
#include "Channel.h"
#include "Socket.h"
#include "util.h"

Connection::Connection(EventLoop *loop, Socket *sock)
    : loop_(loop), sock_(sock), channel_(nullptr), read_buffer_(nullptr) {
  channel_ = new Channel(loop_, sock->GetFd());
  channel_->EnableRead();
  channel_->UseET();
  std::function<void()> cb = std::bind(&Connection::Echo, this, sock->GetFd());
  channel_->SetReadCallback(cb);
  read_buffer_ = new Buffer();
}

Connection::~Connection() {
  delete channel_;
  delete sock_;
  delete read_buffer_;
}

void Connection::SetDeleteConnectionCallback(std::function<void(int)> const &callback) {
  delete_connectioin_callback_ = callback;
}

void Connection::Echo(int sockfd) {
  char buf[1024];  // 这个buf大小无所谓
  while (true) {  // 由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
    memset(&buf, 0, sizeof(buf));
    ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
    if (bytes_read > 0) {
      read_buffer_->Append(buf, bytes_read);
    } else if (bytes_read == -1 && errno == EINTR) {  // 客户端正常中断、继续读取
      printf("continue reading\n");
      continue;
    } else if (bytes_read == -1 &&
               ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {  // 非阻塞IO，这个条件表示数据全部读取完毕
      printf("message from client fd %d: %s\n", sockfd, read_buffer_->ToStr());
      // ErrorIf(write(sockfd, readBuffer->c_str(), readBuffer->size()) == -1,
      // "socket write error");
      Send(sockfd);
      read_buffer_->Clear();
      break;
    } else if (bytes_read == 0) {  // EOF，客户端断开连接
      printf("EOF, client fd %d disconnected\n", sockfd);
      delete_connectioin_callback_(sockfd);
      break;
    } else {
      printf("Connection reset by peer\n");
      delete_connectioin_callback_(sockfd);
      break;
    }
  }
}

void Connection::Send(int sockfd) {
  char buf[read_buffer_->Size()];
  strcpy(buf, read_buffer_->ToStr());  // NOLINT
  int data_size = read_buffer_->Size();
  int data_left = data_size;
  while (data_left > 0) {
    ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left);
    if (bytes_write == -1 && errno == EAGAIN) {
      break;
    }
    data_left -= bytes_write;
  }
}
