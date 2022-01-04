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

#include <cstring>
#include <iostream>
Buffer::Buffer() = default;

Buffer::~Buffer() = default;

void Buffer::append(const char *_str, int _size) {
  for (int i = 0; i < _size; ++i) {
    if (_str[i] == '\0') {
      break;
    }
    buf.push_back(_str[i]);
  }
}

ssize_t Buffer::size() { return buf.size(); }

const char *Buffer::c_str() { return buf.c_str(); }

void Buffer::clear() { buf.clear(); }

void Buffer::getline() {
  buf.clear();
  std::getline(std::cin, buf);
}

void Buffer::setBuf(const char *_buf) {
  buf.clear();
  buf.append(_buf);
}
