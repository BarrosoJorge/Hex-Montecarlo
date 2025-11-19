#ifndef RULES_H
#define RULES_H

#include "../Board/board.h" 

// Logica de victoria
int board_test_x(const char* board, int size);
int board_test_o(const char* board, int size);
char board_test(const char* board, int size);


#endif //RULES_H