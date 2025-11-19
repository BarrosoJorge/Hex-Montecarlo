#include "square.h"

int to_ind(int size, int x, int y) {
    if((x < 0) || (x >= size)) return -1;
    if((y < 0) || (y >= size)) return -1;
    return size * y + x;
}

void to_xy(int size, int ind, int* x, int* y) {
    *y = ind / size;
    *x = ind % size;
}