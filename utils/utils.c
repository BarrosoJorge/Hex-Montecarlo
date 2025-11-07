#include "utils.h"
#include "board.h"  
#include <ctype.h>   
#include <stdio.h>   

int to_indice(int size, int x, int y){
    if ((x < 0) || (x >= size)) return -1;
    if ((y < 0) || (y >= size)) return -1;
    return size * y + x;
}

void to_xy(int size, int indice, int* x, int* y){
    *y = indice/size;
    *x = indice % size;
}

int parse_and_validate_move(const Board* board, const char* input_str) {
    int size = board->size;
    char col_char;
    int row_num;

    if (sscanf(input_str, " %c%d", &col_char, &row_num) != 2) {
        return -1; // Formato inválido
    }

    // 2. Convertir a coordenadas (x, y) internas (base 0)
    int x = toupper(col_char) - 'A'; // 'A' -> 0, 'B' -> 1
    int y = row_num - 1;             // '1' -> 0, '2' -> 1

    // 3. Validar límites (usando la lógica de to_indice)
    if (x < 0 || x >= size || y < 0 || y >= size) {
        return -1; // Fuera de límites
    }

    // 4. Obtener el índice 1D
    int indice = to_indice(size, x, y);

    // 5. Validar si la casilla está vacía
    if (board->squares[indice].simbolo != '+') {
        return -2; // Casilla ocupada
    }

    // ¡Éxito!
    return indice;
}