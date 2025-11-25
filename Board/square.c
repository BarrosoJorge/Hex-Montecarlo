#include "square.h"
#include "utils.h"
#include <stdio.h>

Square crear_square(int indice){
    //Crea un square vacio (simbolo '+' y color 0) 
    Square square;
    square.indice = indice;

    square.color = 0;

    square.simbolo = '+';

    return square;
}


void imprimir_square(const Square* square){
    // Formato: " %c  " (4 caracteres) para alinear con encabezados de consola  
    printf(" %c  ", square->simbolo);
}

void actualizar_square(Square* square, char simbolo, int color){
    // Actualiza los atributos del square con la nueva jugada
    square->simbolo = simbolo;
    square->color = color;
}