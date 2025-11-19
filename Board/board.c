#include "board.h"

void board_clean(char* board, int size) {
    for (int i = 0; i < (size * size); i++) board[i] = '+';
}

void board_print(const char* board, int size) {
    printf("  ");
    for(int n = 0; n < size; n++) {
        printf("%c   ", 'A' + n);
    }
    printf("\n");

    for(int j = 0; j < size; j++) {
        for(int n = 0; n < j; n++) printf("  ");
        printf("%2d  ", j + 1);
        for(int i = 0; i < size; i++) {
            printf("%c   ", board[size * j + i]);
        }
        printf("\n");
    }
}