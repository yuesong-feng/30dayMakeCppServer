/**
 * @file Channel.h
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

class Channel {
 public:
  DISALLOW_COPY_AND_MOVE(Channel);
  Channel(int fd, EventLoop *loop);
  ~Channel();


  void HandleEvent() const;
  void EnableRead();
  void EnableWrite();

  int fd() const;
  short listen_events() const;
  short ready_events() const;
  bool exist() const;
  void set_exist(bool in = true);
  void EnableET();

  void set_ready_event(short ev);
  void set_read_callback(std::function<void()> const &callback);
  void set_write_callback(std::function<void()> const &callback);

  static const short READ_EVENT;
  static const short WRITE_EVENT;
  static const short ET;

 private:
  int fd_;
  EventLoop *loop_;
  short listen_events_;
  short ready_events_;
  bool exist_;
  std::function<void()> read_callback_;
  std::function<void()> write_callback_;
};
