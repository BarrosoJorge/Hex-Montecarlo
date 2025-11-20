#ifndef BOARD_H
#define BOARD_H

#include "square.h"

#define MAX_BOARD_SIDE 26
#define MAX_BOARD_SIZE (MAX_BOARD_SIDE*MAX_BOARD_SIDE)


//Probablemente se mueva a otro archivo
#define MAX_STACK_SIZE (MAX_BOARD_SIZE/2)

typedef struct{
    int size;
    Square squares[MAX_BOARD_SIZE];
    
} Board;

void board_print(const Board* board);

void board_init(Board* board, int size);
void board_destroy(Board* board);




#endif