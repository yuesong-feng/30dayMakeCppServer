/**
 * @file util.cpp
 * @author 冯岳松 (yuesong-feng@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2022-01-04
 *
 * @copyright Copyright (冯岳松) 2022
 *
 */
#include "util.h"

#include <cstdio>
#include <cstdlib>

void ErrorIf(bool condition, const char *errmsg) {
  if (condition) {
    perror(errmsg);
    exit(EXIT_FAILURE);
  }
}
