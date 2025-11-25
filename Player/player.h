#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h> // Necesario para int64_t
#include "board.h" 


// Definición de puntero a función para polimorfismo (Humano vs IA)
typedef int (*get_move_callback_t)(const Board* board, char player_symbol);

typedef struct {
    char simbolo; // 'X' o 'O'
    int color;    // 1 o 2
    
    // Esta variable 'apunta' a la función que el jugador usa
    get_move_callback_t get_move; 
} Player;

/*
Solicita un movimiento al usuario mediante la consola. se usa en consola
    board: puntero al tablero actual
    player_symbol: 'X' u 'O'
*/
int get_human_move(const Board* board, char player_symbol);

/*
Calcula el mejor movimiento usando Monte Carlo en multiproceso.
Coordina el Process Manager y mide el tiempo de respuesta.
    board: puntero al tablero actual
    player_symbol: 'X' u 'O'
*/
int get_ai_move_montecarlo(const Board* board, char player_symbol);

/*
Simula una partida completa al azar desde el estado actual.
Retorna 'X' u 'O' segun quien gane.
    board: array de caracteres del tablero
    size: tamaño del tablero
    player: a quien le toca jugar el siguiente movimiento aleatorio
*/
char game_sim(const char* board, int size, char player);

/*
Ejecuta múltiples simulaciones para evaluar movimientos posibles.
Se ejecuta dentro de los procesos hijos (workers).
    board: array de caracteres
    size: tamaño del tablero
    player: jugador evaluando
    nsim: numero de simulaciones a correr por casilla vacia
    stat: array donde se guardara el puntaje de cada casilla
*/
void game_stats(const char* board, int size, char player, int64_t nsim, int64_t* stat);

/*
Selecciona el índice con mayor puntaje en el array de estadísticas.
    stats: array con los resultados de las simulaciones
    board: para verificar que la casilla elegida no este ocupada (seguridad)
    size: tamaño del tablero
*/
int game_move(int64_t* stats, const char* board, int size);
#endif //PLAYER_H
