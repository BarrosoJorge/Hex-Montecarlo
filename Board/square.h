#ifndef SQUARE_H
#define SQUARE_H


typedef struct {
    int indice; //posicion en un array unidimensional
    int color; // 0: vacio 1: blanco 2:negro 
    char simbolo; // '+' 'X' 'O'
} Square;
//Simbolo sirve para el jugador (si se juega sin interfaz visual)
//Color sirve para el estado del hexagono para la interfaz visual

//Funciones

/*
Crea una nueva casilla: 
    indice: posicion del arreglo 
    size: tamaño del tablero 
*/
Square crear_square(int indice);

/*
Imprime una casilla en la consola
    square: puntero a la casilla a imprimir
*/
void imprimir_square(const Square* square);

//Implementrar cuando se tenga la logia del juego /* turno */
void actualizar_square(Square* square, char simbolo, int color);

#endif
