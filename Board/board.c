#include <stdio.h>
#include "board.h"
#include "utils.h"

void board_print(const Board* board) {
    int size = board->size;

    //Imprimir Encabezado de Columnas 
    
    // Espacio inicial para alinear con los números de fila 
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

            // Imprimir número de fila (ej. " 1 ", " 2 ", "10 ")
            printf("%2d ", y + 1);

            // Imprimir la indentación (offset) para la forma de rombo
            // Usamos printf con ancho variable para evitar un loop explícito.
            // Imprime 'y * 2' espacios.
            printf("%*s", y * 2, ""); 
        }

        // --- Lógica de Impresión de Casilla ---
        imprimir_square(&board->squares[i]);

        // --- Lógica de FIN de Fila ---
        // Si x es la última columna, imprimir un salto de línea.
        if (x == size - 1) {
            printf("\n");
        }
    }
}

void board_init(Board* board, int size) {
    board->size = size;
    for (int i = 0; i < size * size; i++) {
        board->squares[i] = crear_square(i, size);
    }
}


void board_destroy(Board* board) {
    // Por ahora no hace nada, ya que no usamos 'malloc' ni memoria dinámica.
    // Pero la función debe existir para que el enlazador la encuentre.
    (void)board; // Para evitar warnings de 'parámetro no usado'
}


