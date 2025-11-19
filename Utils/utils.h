#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include <stdlib.h>
#include "../Board/square.h"

void trim(char* str);
int read_move(const char* buffer, int size);

#endif