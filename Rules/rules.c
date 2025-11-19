#include "rules.h"
#include "../Board/square.h"

int board_test_x(const char* board, int size) {
    int stack[MAX_STACK_SIZE];
    char bcopy[MAX_BOARD_SIZE];
    int k = 0; // Indice del stack

    for(int i = 0; i < (size * size); i++) bcopy[i] = board[i];

    /* Buscamos tokens X (horizontales) */
    for(int i = 0; i < size; i++) {
        if(bcopy[i] == 'X') {
            stack[k++] = i;
            bcopy[i] = '+';
        }
    }

    if(k == 0) return 0; // No hay una ruta

    int x, y;
    int nb[6];
    while(k > 0) {
        int pos = stack[--k];
        to_xy(size, pos, &x, &y);
        if(y == size-1) return 1;

        nb[0] = to_ind(size, x, y-1);
        nb[1] = to_ind(size, x+1, y-1);
        nb[2] = to_ind(size, x+1, y);
        nb[3] = to_ind(size, x, y+1);
        nb[4] = to_ind(size, x-1, y+1);
        nb[5] = to_ind(size, x-1, y);

        for(int i = 0; i < 6; i++) {
            if(nb[i] < 0) continue; 
            
            if(bcopy[nb[i]] == 'X') {
                stack[k++] = nb[i];
                bcopy[nb[i]] = '+';
            }
        }
    }
    return 0;
}

int board_test_o(const char* board, int size) {
    int stack[MAX_STACK_SIZE];
    char bcopy[MAX_BOARD_SIZE];
    int k = 0;

    for(int i = 0; i < (size * size); i++) bcopy[i] = board[i];

    /* Buscamos tokens 0 (verticales) */
    for(int i = 0; i < (size * size); i+=size) {
        if(bcopy[i] == 'O') {
            stack[k++] = i;
            bcopy[i] = '+';
        }
    }

    if(k == 0) return 0;// No hay una ruta

    int x, y;
    int nb[6];
    while(k > 0) {
        int pos = stack[--k];
        to_xy(size, pos, &x, &y);
        if(x == size-1) return 1;

        nb[0] = to_ind(size, x, y-1);
        nb[1] = to_ind(size, x+1, y-1);
        nb[2] = to_ind(size, x+1, y);
        nb[3] = to_ind(size, x, y+1);
        nb[4] = to_ind(size, x-1, y+1);
        nb[5] = to_ind(size, x-1, y);

        for(int i = 0; i < 6; i++) {
            if(nb[i] < 0) continue; 
            
            if(bcopy[nb[i]] == 'O') {
                stack[k++] = nb[i];
                bcopy[nb[i]] = '+';
            }
        }
    }
    return 0;
}

char board_test(const char* board, int size) {
    if(board_test_o(board, size)) return 'O';
    if(board_test_x(board, size)) return 'X';
    return '+';
}