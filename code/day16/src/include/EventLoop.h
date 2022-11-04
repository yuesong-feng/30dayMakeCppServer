/**
 * @file EventLoop.h
 * @author 冯岳松 (yuesong-feng@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2022-01-04
 *
 * @copyright Copyright (冯岳松) 2022
 *
 */
#pragma once
#include "common.h"
#include <memory>

class EventLoop {
 public:
  DISALLOW_COPY_AND_MOVE(EventLoop);
  EventLoop();
  ~EventLoop();


  void Loop() const;
  void UpdateChannel(Channel *ch) const;
  void DeleteChannel(Channel *ch) const;

 private:
  std::unique_ptr<Poller> poller_;
};
