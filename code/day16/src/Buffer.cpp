/**
 * @file Buffer.cpp
 * @author 冯岳松 (yuesong-feng@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2022-01-04
 *
 * @copyright Copyright (冯岳松) 2022
 *
 */
#include "Buffer.h"

const std::string &Buffer::buf() const { return buf_; }

const char *Buffer::c_str() const { return buf_.c_str(); }

void Buffer::set_buf(const char *buf) {
  std::string new_buf(buf);
  buf_.swap(new_buf);
}

size_t Buffer::Size() const { return buf_.size(); }

void Buffer::Append(const char *str, int size) {
  for (int i = 0; i < size; ++i) {
    if (str[i] == '\0') {
      break;
    }
    buf_.push_back(str[i]);
  }
}

void Buffer::Clear() { buf_.clear(); }
