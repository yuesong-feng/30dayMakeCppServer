/**
 * @file Buffer.h
 * @author 冯岳松 (yuesong-feng@foxmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-04
 * 
 * @copyright Copyright (冯岳松) 2022
 * 
 */
#pragma once
#include <string>
#include "Macros.h"

class Buffer {
 public:
  Buffer();
  ~Buffer();

  void append(const char *_str, int _size);
  ssize_t size();
  const char *c_str();
  void clear();
  void getline();
  void setBuf(const char *);

 private:
  std::string buf;
};
