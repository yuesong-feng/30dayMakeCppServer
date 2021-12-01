#include "util.h"
#include <stdio.h>
#include <stdlib.h>

void errif(bool condition, const char *errmsg){
    perror(errmsg);
    exit(EXIT_FAILURE);
}