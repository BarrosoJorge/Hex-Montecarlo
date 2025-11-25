#ifndef UTILS_H
#define UTILS_H

#include "board.h"

/*
Convierte coordenadas 2D (x, y) a un indice 1D.
Retorna -1 si las coordenadas estan fuera del tablero.
    size: tamaño del lado del tablero
    x: columna (0 a size-1)
    y: fila (0 a size-1)
*/
int to_indice(int size, int x, int y);

/*
Convierte un indice 1D a coordenadas 2D (x, y).
    size: tamaño del lado del tablero
    indice: posicion en el array lineal
    x: puntero donde se guardara la columna
    y: puntero donde se guardara la fila
*/
void to_xy(int size, int indice, int* x, int* y);

/*
Analiza un string (ej. "A1", "C4") y valida si es una jugada legal.
Retorna el indice si es valido.
Retorna -1 si el formato es incorrecto o fuera de limites.
Retorna -2 si la casilla ya esta ocupada.
    board: puntero al tablero actual
    input_str: cadena de texto ingresada por el usuario
*/
int parse_and_validate_move(const Board* board, const char* input_str);

/*
Crea una representacion simplificada del tablero (array de chars).
    src: puntero al tablero estructurado (origen)
    dest: puntero al array de caracteres (destino). Debe tener tamaño MAX_BOARD_SIZE.
*/
void convert_board_to_char(const Board* src, char* dest);
#endif

