#include "square.h"
#include "utils.h"
#include <stdio.h>

Square crear_square(int indice, int size){
    //Crea un square vacio (simbolo '+' y color 0) 

    //formula para calcular x e y a partir del indice
    int x, y;
    to_xy(size, indice, &x, &y);
    Square square;
    square.indice = indice;
    square.x = x;
    square.y = y;


    square.color = 0;

    square.simbolo = '+';

    return square;
}

void destruir_square(Square* square) {
    //No hay memoria dinamica por ahora, pero se deja para futuras implementaciones
    //Si se agrega memoria dinamica, liberar aqui
    (void)square; // Evitar warning de variable no usada
}

void imprimir_square(const Square* square){
    // Implementación:
    // Imprime el símbolo del square con un formato consistente.
    // Usamos " %c  " (4 caracteres total) para que coincida con
    // el espaciado de las cabeceras (ej. " A  ").
    
    printf(" %c  ", square->simbolo);
}

void actualizar_square(Square* square, char simbolo, int color){
    // Actualiza los atributos del square con la nueva jugada
    square->simbolo = simbolo;
    square->color = color;
}