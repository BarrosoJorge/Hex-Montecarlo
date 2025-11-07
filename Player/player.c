#include "player.h"
#include "utils.h"  // Necesitaremos la función de parseo
#include <stdio.h>   // Para printf, fgets
#include <string.h>  // Para strcspn (para quitar saltos de línea)
#include <unistd.h> // Asegúrate de tener este include en player.c para sleep()



int get_human_move(const Board* board, char player_symbol) {
    char buffer[32]; // Un buffer para leer la entrada del usuario
    int indice = -1;

    // Bucle infinito hasta que el usuario dé una entrada válida
    while (1) {
        printf("Turno de %c. Introduce jugada (ej. C4): ", player_symbol);
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            // Manejar error de entrada (ej. EOF)
            continue; 
        }

        // Quitar el salto de línea ('\n') que fgets captura
        buffer[strcspn(buffer, "\n")] = 0;

        // Llamar a nuestra nueva función de 'utils' para validar
        indice = parse_and_validate_move(board, buffer);

        if (indice == -1) {
            printf("Error: Formato inválido. Usa LetraNumero (ej. A1, C4, G7).\n");
        } else if (indice == -2) {
            printf("Error: Casilla (%s) ya está ocupada. Elige otra.\n", buffer);
        } else {
            break; 
        }
    }
    
    return indice;
}



int get_ai_move_montecarlo(const Board* board, char player_symbol) {
    
    // 1. Simular que "piensa" (mejora la experiencia)
    printf("IA (%c) está pensando...\n", player_symbol);
    sleep(1); // Pausa de 1 segundo

    // 2. Encontrar el primer movimiento (tu lógica)
    int board_total_size = board->size * board->size;
    for (int i = 0; i < board_total_size; i++) {
        
        // Usamos tu comprobación, que es correcta
        if (board->squares[i].color == 0) { 
            
            // 3. Anunciar el movimiento (mejora la experiencia)
            int x, y;
            to_xy(board->size, i, &x, &y);
            printf("IA (%c) ha elegido: %c%d\n", player_symbol, 'A' + x, y + 1);

            return i; // Devolver el primer índice vacío
        }
    }

    return -1; // No hay movimientos (tablero lleno)
}