#ifndef BOARD_H
#define BOARD_H

#include "square.h"

#define MAX_BOARD_SIDE 26
#define MAX_BOARD_SIZE (MAX_BOARD_SIDE*MAX_BOARD_SIDE)


typedef struct{
    int size; //tamaño del lado del tablero
    Square squares[MAX_BOARD_SIZE]; //arreglo unidimensional de casillas
    
} Board;

/*
Imprime el tablero en la consola con formato de rombo.
    board: puntero al tablero a imprimir
*/
void board_print(const Board* board);

/*
Inicializa un tablero del tamaño especificado.
Llenando las casillas con valores vacios.
    board: puntero al tablero a inicializar
    size: tamaño del lado del tablero
*/
void board_init(Board* board, int size);





#endif