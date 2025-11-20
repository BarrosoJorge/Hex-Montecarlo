#include "player.h"
#include "utils.h"  // Necesitaremos la función de parseo
#include "rules.h" // Para board_status
#include "pcg_basic.h" // Para pcg32_boundedrand
#include <stdio.h>   // Para printf, fgets
#include <string.h>  // Para strcspn (para quitar saltos de línea)
#include <unistd.h> // Asegúrate de tener este include en player.c para sleep()
#include <process_manager.h> // Para usar procesos

int64_t nsim = 1000; // Hardcoded por ahora
//Revisar
int get_human_move(const Board* board, char player_symbol) {
    char buffer[32]; // Un buffer para leer la entrada del usuario
    int indice = -1;

    // Bucle infinito hasta que el usuario dé una entrada válida
    while (1) {
        printf("Turno de %c. Introduce jugada (ej. C4): ", player_symbol);

        // Agregar que reciba minúsculas también
        
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


// Simula un juego aleatorio (Random Playgout)
char game_sim(const char* board, int size, char player) {
    char bcopy[MAX_BOARD_SIZE];

    // 1. Copia local rápida (stack)
    int total_squares = size * size;
    for(int i = 0; i < total_squares; i++) bcopy[i] = board[i];

    char turn = player;
    int status;
    int move;

    while(1) {
        // RNG Rápido (PCG) para elegir casilla
        move = pcg32_boundedrand(total_squares);
        
        // Si está ocupada, intentar otro random
        if(bcopy[move] != '+') continue;

        // Hacer movimiento
        bcopy[move] = turn;

        // Verificar victoria usando tus Reglas (Rules/rules.h)
        status = board_status(bcopy, size);
        
        // board_status devuelve: 0=Nadie, 1=X, 2=O
        if(status == 1) return 'X';
        if(status == 2) return 'O';

        // Cambiar turno
        turn = (turn == 'X') ? 'O' : 'X';
    }
    return '+'; // Nunca debería llegar aquí en Hex (no hay empates)
}

// Ejecuta múltiples simulaciones
void game_stats(const char* board, int size, char player, int64_t nsim, int64_t* stat) {
    char bcopy[MAX_BOARD_SIZE];
    int total_squares = size * size;

    // Copia local
    for(int i = 0; i < total_squares; i++) bcopy[i] = board[i];

    // Limpiar estadísticas
    for(int i = 0; i < total_squares; i++) stat[i] = 0;

    char winner;
    char other = (player == 'X') ? 'O' : 'X';

    // Bucle principal de simulaciones
    // NOTA: Aquí es donde irían los Procesos más adelante.
    // Por ahora, se ejecuta secuencial (lento pero funcional).
    
    for(int k = 0; k < total_squares; k++) {
        // Solo evaluamos casillas vacías
        if(bcopy[k] != '+') continue;

        // Para cada casilla vacía, simulamos 'nsim' juegos
        for(int64_t n = 0; n < nsim; n++) { 
            bcopy[k] = player; // Hacemos el movimiento candidato
            
            // Simulamos el resto del juego
            winner = game_sim(bcopy, size, other);
            
            bcopy[k] = '+'; // Deshacemos el movimiento (backtrack)

            if(winner == player) stat[k]++;
            else stat[k]--;
        }
    }
}

// Elige el mejor movimiento basado en stats
int game_move(int64_t* stats, const char* board, int size) {
    int best = -1;
    int total_squares = size * size;

    for(int i = 0; i < total_squares; i++) {
        if(board[i] != '+') continue;
        
        // Algoritmo simple de máximo
        if((best == -1) || (stats[i] > stats[best])) {
            best = i;
        }
    }
    return best;
}


int get_ai_move_montecarlo(const Board* board, char player_symbol) {
    printf("IA (%c) pensando en paralelo...\n", player_symbol);
    
    // 1. Convertir a formato crudo
    char raw_board[MAX_BOARD_SIZE];
    convert_board_to_char(board, raw_board);
    
    // Limpiamos el resto del buffer para evitar basura
    for(int i = board->size * board->size; i < MAX_BOARD_SIZE; i++) raw_board[i] = '+';

    // 2. DELEGAR AL PROCESS MANAGER
    // Ya no llamamos a game_stats aquí directamente.
    int best_move_index = processes_get_best_move(raw_board, board->size, player_symbol);

    // 3. Feedback
    if (best_move_index != -1) {
        int x, y;
        to_xy(board->size, best_move_index, &x, &y);
        printf("IA (%c) decide jugar en: %c%d\n", 
               player_symbol, 'A' + x, y + 1);
    }

    return best_move_index;
}
