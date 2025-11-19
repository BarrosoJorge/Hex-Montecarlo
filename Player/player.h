#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include "../Board/board.h" 

// Simulacion y Estadisticas
char game_sim(const char* board, int size, char player);
void game_stats(const char* board, int size, char player, int64_t nsim, int64_t* stat);
int game_move(int64_t* stats, const char* board, int size);

#endif //PLAYER_H