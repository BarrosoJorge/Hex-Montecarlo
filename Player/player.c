#include "player.h"
#include "utils.h"  
#include "rules.h" 
#include "pcg_basic.h" 
#include <process_manager.h> 

#include <stdio.h>   // Para printf, fgets
#include <string.h>  // Para strcspn (para quitar saltos de línea)
#include <sys/time.h>




// Algoritmo Fisher-Yates para desordenar un array 
static void shuffle(int *array, int n) {
    if (n > 1) {
        for (int i = 0; i < n - 1; i++) {
            // Usamos el generador rápido PCG
            int j = i + pcg32_boundedrand(n - i);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}


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

char game_sim(const char* board, int size, char player) {
    char bcopy[MAX_BOARD_SIZE];
    int empty_squares[MAX_BOARD_SIZE];
    int count = 0;
    int total_squares = size * size;

    // 1. Copia rápida y detección de vacíos
    for(int i = 0; i < total_squares; i++) {
        bcopy[i] = board[i];
        if(bcopy[i] == '+') {
            empty_squares[count++] = i;
        }
    }

    if (count == 0) {
        if (board_test_x(bcopy, size)) return 'X';
        return 'O';
    }

    // 2. BARAJAR las posiciones vacías
    // Esto simula que los turnos ocurren al azar, pero mucho más rápido
    // que elegir uno por uno y verificar victoria cada vez.
    shuffle(empty_squares, count);

    // 3. LLENAR el tablero completo
    char current_turn = player;
    for(int i = 0; i < count; i++) {
        bcopy[empty_squares[i]] = current_turn;
        current_turn = (current_turn == 'X') ? 'O' : 'X';
    }

    // 4. VERIFICAR VICTORIA UNA SOLA VEZ AL FINAL
    // En Hex no hay empates. Si X no ganó, forzosamente ganó O.
    if (board_test_x(bcopy, size)) return 'X';
    else return 'O';
}

void game_stats(const char* board, int size, char player, int64_t nsim, int64_t* stat) {
    int total_squares = size * size;
    
    // Limpiar estadísticas
    for(int i = 0; i < total_squares; i++) stat[i] = 0;

    char winner;
    char other = (player == 'X') ? 'O' : 'X';
    
    // Buffer local para no modificar el original
    char local_board[MAX_BOARD_SIZE];
    
    // Copia inicial rápida usando memcpy (más rápido que bucle for)
    memcpy(local_board, board, MAX_BOARD_SIZE);

    for(int k = 0; k < total_squares; k++) {
        // Solo evaluamos casillas válidas
        if(local_board[k] != '+') continue;

        // Simulamos 'nsim' juegos para esta casilla
        for(int64_t n = 0; n < nsim; n++) { 
            // Hacemos el movimiento hipotético
            local_board[k] = player; 
            
            // Corremos la simulación rápida
            winner = game_sim(local_board, size, other);
            
            // Deshacemos el movimiento (Backtracking)
            local_board[k] = '+'; 

            if(winner == player) stat[k]++;
            else stat[k]--;
        }
    }
}


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
    //printf("IA (%c) pensando en paralelo...\n", player_symbol);
    
    // 1. Convertir a formato crudo
    char raw_board[MAX_BOARD_SIZE];
    convert_board_to_char(board, raw_board);
    
    // Limpiamos el resto del buffer para evitar basura
    for(int i = board->size * board->size; i < MAX_BOARD_SIZE; i++) raw_board[i] = '+';

    // 1. INICIO DEL CRONÓMETRO
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // 2. DELEGAR AL PROCESS MANAGER
    int best_move_index = processes_get_best_move(raw_board, board->size, player_symbol);

    gettimeofday(&end, NULL);
    double elapsed_time = (end.tv_sec - start.tv_sec) + 
                          (end.tv_usec - start.tv_usec) / 1000000.0;
    
    
    printf("   TIEMPO DE RESPUESTA IA: %.4f segundos\n", elapsed_time);
    // 3. Feedback
    if (best_move_index != -1) {
        int x, y;
        to_xy(board->size, best_move_index, &x, &y);
        printf("IA (%c) decide jugar en: %c%d\n", 
               player_symbol, 'A' + x, y + 1);
    }

    return best_move_index;
}
