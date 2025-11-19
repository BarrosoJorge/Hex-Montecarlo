#ifndef BOARD_H
#define BOARD_H

#include <stdio.h>
#include <stdlib.h>

// Constantes Globales del Tablero
#define MAX_BOARD_SIDE 26
#define MAX_BOARD_SIZE (MAX_BOARD_SIDE*MAX_BOARD_SIDE)
#define MAX_STACK_SIZE (MAX_BOARD_SIZE / 2)

// Definicion de Fichas 
typedef enum {
    none, white, black
} token_t;

// Funciones del Tablero
void board_clean(char* board, int size);
void board_print(const char* board, int size);

#endif