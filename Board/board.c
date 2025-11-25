#include <stdio.h>
#include "board.h"
#include "utils.h"


void board_print(const Board* board) {
    int size = board->size;

    //Imprimir Encabezado de Columnas 
    printf("   "); 
    for (int i = 0; i < size; i++) {
        // Imprime " A  ", " B  ", 
        printf(" %c  ", 'A' + i);
    }
    printf("\n");



    // Iteramos por cada casilla en el array (0 hasta size*size - 1)
    for (int i = 0; i < size * size; i++) {
        
        // Calculamos 'x' (columna actual) usando módulo
        int x = i % size;


        // Si x es 0, estamos al principio de una nueva fila.
        if (x == 0) {
            // Calculamos 'y' (fila actual)
            int y = i / size;

            // Imprimir número de fila
            printf("%2d ", y + 1);

            // Imprimir la indentación (offset) para la forma de rombo
            // Imprime 'y * 2' espacios.
            printf("%*s", y * 2, ""); 
        }

        imprimir_square(&board->squares[i]);

        // Si x es la última columna, imprimir un salto de línea.
        if (x == size - 1) {
            printf("\n");
        }
    }
}

void board_init(Board* board, int size) {
    board->size = size;
    for (int i = 0; i < size * size; i++) {
        board->squares[i] = crear_square(i);
    }
}

