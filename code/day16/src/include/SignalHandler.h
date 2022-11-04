/**
 * @file Signal.h
 * @author 冯岳松 (yuesong-feng@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2022-02-07
 *
 * @copyright Copyright (冯岳松) 2022
 *
 */
#pragma once
#include <signal.h>
#include <functional>
#include <map>

std::map<int, std::function<void()>> handlers_;
void signal_handler(int sig) {
  handlers_[sig]();
}

struct Signal {
  static void signal(int sig, const std::function<void()> &handler) {
    handlers_[sig] = handler;
    ::signal(sig, signal_handler);
  }
};