#ifndef RULES_H
#define RULES_H

#include "board.h" 


int board_status(const char* board, int size);

int  board_test_x(const char* board, int size);

int  board_test_o(const char* board, int size);


#endif //RULES_H