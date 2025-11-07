#ifndef SQUARE_H
#define SQUARE_H


typedef struct {
    int x;
    int y;
    int indice; //posicion en un array unidimensional
    int color; // 0: vacio 1: blanco 2:negro 
    char simbolo; // '+' 'X' 'O'
} Square;
//Simbolo (temporalmente hasta implementar interfaz visual) sirve para el jugador
//Color sirve para el estado del hexagono para el programador

//Funciones

Square crear_square(int indice, int size);
void destruir_square(Square* square);

void imprimir_square(const Square* square);

//Implementrar cuando se tenga la logia del juego /* turno */
void actualizar_square(Square* square, char simbolo, int color);

#endif
