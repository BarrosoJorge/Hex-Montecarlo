#include "rules.h"
#include "board.h"

#include "rules.h"
#include "board.h"  
#include "utils.h"  

#define MAX_STACK_SIZE (MAX_BOARD_SIZE/2)

int  board_test_x(const char* board, int size){
    int pila[MAX_STACK_SIZE];
    char copy[MAX_BOARD_SIZE];
    int k = 0;

    for (int i = 0; i < size*size; i++)
    {
        copy[i] = board[i];
    }

    //Buscamos tokens X (horizontales) en la fila superior (y=0)
    for (int i = 0; i < size; i++)
    {
        if (copy[i] == 'X' ) {
            pila[k++] = i;
            copy[i] = '+'; //Marcamos como visitado
        }
    }

    if (k == 0) return 0; // No hay ruta
    
    int x, y;
    int vecinos[6];
    while(k > 0){
        int posicion = pila[--k];
        to_xy(size, posicion, &x, &y);
        
        // Si llegamos a la última fila (y == size - 1), X gana
        if(y == size - 1) return 1;

        vecinos[0] = to_indice(size, x,     y-1 ); // Arriba
        vecinos[1] = to_indice(size, x+1,   y-1 ); // Arriba-Derecha
        vecinos[2] = to_indice(size, x+1,   y   ); // Derecha
        vecinos[3] = to_indice(size, x,     y+1 ); // Abajo
        vecinos[4] = to_indice(size, x-1,   y+1 ); // Abajo-Izquierda
        vecinos[5] = to_indice(size, x-1,   y   ); // Izquierda

        for (int i = 0; i < 6; i++) {
            // to_indice devuelve -1 si está fuera de límites
            if (vecinos[i] < 0 ) continue;

            if (copy[vecinos[i]] == 'X') {
                pila[k++] = vecinos[i];
                copy[vecinos[i]] = '+'; //Marcamos como visitado
            }
        }
    }

    return 0; // No se encontró camino
}


int  board_test_o(const char* board, int size){
    int pila[MAX_STACK_SIZE];
    char copy[MAX_BOARD_SIZE];
    int k = 0;

    for (int i = 0; i < size*size; i++)
    {
        copy[i] = board[i];
    }
    
    //Buscamos tokens O (verticales) en la primera columna (x=0)
    for (int i = 0; i < size*size; i+=size)
    {
        if (copy[i] == 'O' ) {
            pila[k++] = i;
            copy[i] = '+'; //Marcamos como visitado
        }
    }

    if (k == 0) return 0; // No hay ruta
    
    int x, y;
    int vecinos[6];
    while(k > 0){
        int posicion = pila[--k];
        to_xy(size, posicion, &x, &y);
        
        // Si llegamos a la última columna (x == size - 1), O gana
        if(x == size - 1) return 1;

        vecinos[0] = to_indice(size, x,     y-1 ); // Arriba
        vecinos[1] = to_indice(size, x+1,   y-1 ); // Arriba-Derecha
        vecinos[2] = to_indice(size, x+1,   y   ); // Derecha
        vecinos[3] = to_indice(size, x,     y+1 ); // Abajo
        vecinos[4] = to_indice(size, x-1,   y+1 ); // Abajo-Izquierda
        vecinos[5] = to_indice(size, x-1,   y   ); // Izquierda

        for (int i = 0; i < 6; i++) {
            // to_indice devuelve -1 si está fuera de límites
            if (vecinos[i] < 0 ) continue;

            if (copy[vecinos[i]] == 'O') {
                pila[k++] = vecinos[i];
                copy[vecinos[i]] = '+'; //Marcamos como visitado
            }
        }
    }

    return 0; // No se encontró camino
}

int board_status(const char* board, int size) {
    if (board_test_x(board, size)) {
        return 1; // 1 = Gana X
    }
    if (board_test_o(board, size)) {
        return 2; // 2 = Gana O
    }
    return 0; // 0 = Sin ganador
}