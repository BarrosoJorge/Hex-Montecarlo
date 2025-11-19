#include "player.h"
#include "../Rules/rules.h"
#include "../pcg_basic.h"

// Simula un juego a partir de la posicion actual del tablero
char game_sim(const char* board, int size, char player) {
    char bcopy[MAX_BOARD_SIZE];

    // Copia local del tablero
    for(int i = 0; i < (size * size); i++) bcopy[i] = board[i];

    char turn = player;
    char out;
    int move;

    while(1) {
        move = pcg32_boundedrand(size * size);
        if(bcopy[move] != '+') continue;

        bcopy[move] = turn;
        out = board_test(bcopy, size);
        if(out != '+') return out;

        turn = (turn == 'X') ? 'O' : 'X';
    }
    return '+';
}

void game_stats(const char* board, int size, char player, int64_t nsim, int64_t* stat) {
    char bcopy[MAX_BOARD_SIZE];

    // Copia local del tablero
    for(int i = 0; i < (size * size); i++) bcopy[i] = board[i];

    // Inicializacion 
    for(int i = 0; i < (size * size); i++) stat[i] = 0;

    // Elegimos donde jugar
    char out;
    char other = (player == 'X') ? 'O' : 'X';

    for(int64_t n = 0; n < nsim; n++) { 
        for(int k = 0; k < (size * size); k++) {
            if(bcopy[k] != '+') continue;

            bcopy[k] = player;
            out = game_sim(bcopy, size, other);
            bcopy[k] = '+';

            if(out == player) stat[k] ++;
            else stat[k] --;
        }
    }
}

int game_move(int64_t* stats, const char* board, int size) {
    int best = -1;
    for(int i = 0; i < (size * size); i++) {
        if(board[i] != '+') continue;
        if((best == -1) || (stats[i] > stats[best])) best = i;
    }
    return best;
}