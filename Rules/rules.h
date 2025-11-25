#ifndef RULES_H
#define RULES_H

#include "board.h" 

/*
Verifica si hay un ganador en el tablero actual.
Retorna:
    0: Sin ganador aun
    1: Gana X (Conexion Vertical / Top-Bottom)
    2: Gana O (Conexion Horizontal / Left-Right)
    
    board: array de caracteres representando el tablero
    size: tamaño del lado del tablero
*/
int board_status(const char* board, int size);
/*
Realiza una busqueda DFS para ver si 'X' ha conectado Arriba con Abajo.
    board: array de caracteres
    size: tamaño del lado
*/
int  board_test_x(const char* board, int size);
/*
Realiza una busqueda DFS para ver si 'O' ha conectado Izquierda con Derecha.
    board: array de caracteres
    size: tamaño del lado
*/
int  board_test_o(const char* board, int size);


#endif //RULES_H