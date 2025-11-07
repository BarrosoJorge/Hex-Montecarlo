#ifndef PLAYER_H
#define PLAYER_H

#include "board.h" 


// Define un 'apodo' para un tipo de dato:
// "Un puntero a una función que devuelve un 'int' (el índice)
// y acepta un 'const Board*' y un 'char' (símbolo)".
typedef int (*get_move_callback_t)(const Board* board, char player_symbol);



typedef struct {
    char simbolo; // 'X' o 'O'
    int color;    // 1 o 2
    
    // Esta variable 'apunta' a la función que el jugador usa
    // para decidir su movimiento (ej. get_human_move)
    get_move_callback_t get_move; 
} Player;


int get_human_move(const Board* board, char player_symbol);

int get_ai_move_montecarlo(const Board* board, char player_symbol);


#endif //PLAYER_H
