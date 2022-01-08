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

#include <cassert>
#include <cstring>
#include <iostream>
#include <utility>

#include "Buffer.h"
#include "Channel.h"
#include "Socket.h"
#include "util.h"

Connection::Connection(EventLoop *loop, Socket *sock) : loop_(loop), sock_(sock) {
  if (loop_ != nullptr) {
    channel_ = new Channel(loop_, sock->GetFd());
    channel_->EnableRead();
    channel_->UseET();
  }
  read_buffer_ = new Buffer();
  send_buffer_ = new Buffer();
  state_ = State::Connected;
}

Connection::~Connection() {
  if (loop_ != nullptr) {
    delete channel_;
  }
  delete sock_;
  delete read_buffer_;
}

void Connection::Recv() {
  assert(state_ == State::Connected);
  read_buffer_->Clear();
  int sockfd = sock_->GetFd();
  char buf[1024];  // 这个buf大小无所谓
  if (sock_->IsNonBlocking()) {
    while (true) {  // 使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
      memset(buf, 0, sizeof(buf));
      ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
      if (bytes_read > 0) {
        read_buffer_->Append(buf, bytes_read);
      } else if (bytes_read == -1 && errno == EINTR) {  // 程序正常中断、继续读取
        printf("continue reading\n");
        continue;
      } else if (bytes_read == -1 &&
                 ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {  // 非阻塞IO，这个条件表示数据全部读取完毕
        break;
      } else if (bytes_read == 0) {  // EOF，客户端断开连接
        printf("EOF, client fd %d disconnected\n", sockfd);
        state_ = State::Closed;
        break;
      } else {
        printf("Other error on client fd %d\n", sockfd);
        state_ = State::Closed;
        break;
      }
    }
  } else {
    ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
    read_buffer_->Append(buf, bytes_read);
  }
}
void Connection::Send() {
  assert(state_ == State::Connected);
  int sockfd = sock_->GetFd();
  char buf[send_buffer_->Size()];
  strcpy(buf, send_buffer_->ToStr());  // NOLINT
  if (sock_->IsNonBlocking()) {
    int data_size = send_buffer_->Size();
    int data_left = data_size;
    while (data_left > 0) {
      ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left);
      if (bytes_write == -1 && errno == EAGAIN) {
        break;
      }
      data_left -= bytes_write;
    }
  } else {
    ssize_t write_bytes = write(sockfd, send_buffer_->ToStr(), send_buffer_->Size());
    if (write_bytes == -1) {
      printf("socket write error\n");
    }
  }
}

Connection::State Connection::GetState() { return state_; }

void Connection::Close() { delete_connectioin_callback_(sock_); }

void Connection::SetSendBuffer(const char *str) { send_buffer_->SetBuf(str); }

Buffer *Connection::GetReadBuffer() { return read_buffer_; }
Buffer *Connection::GetSendBuffer() { return send_buffer_; }

void Connection::SetDeleteConnectionCallback(std::function<void(Socket *)> const &callback) {
  delete_connectioin_callback_ = callback;
}
void Connection::SetOnConnectCallback(std::function<void(Connection *)> const &callback) {
  on_connect_callback_ = callback;
  channel_->SetReadCallback([this]() { on_connect_callback_(this); });
}
