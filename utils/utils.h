#ifndef UTILS_H
#define UTILS_H

#include "board.h"

int to_indice(int size, int x, int y);
void to_xy(int size, int indice, int* x, int* y);
int parse_and_validate_move(const Board* board, const char* input_str);
void convert_board_to_char(const Board* src, char* dest);
#endif

