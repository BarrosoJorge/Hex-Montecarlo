#include <stdio.h>
#include <stdlib.h>
#include "board.h"   // Para Board, board_init, board_print, actualizar_square
#include "player.h"  // Para Player, get_human_move, get_ai_move...
#include "rules.h"   // ¡NUEVO! Para board_status

int main() {

    int board_size = 7; // Define el tamaño del tablero
    Board board;
    board_init(&board, board_size);

    // --- 1. Inicializar Jugadores ---
    Player p1 = {'X', 1, get_human_move}; 
    Player p2 = {'O', 2, get_ai_move_montecarlo};

    printf("--- ¡Bienvenido a Hex! ---\n");
    printf("Jugador 1: %c (Humano)\n", p1.simbolo);
    printf("Jugador 2: %c (IA)\n", p2.simbolo);
    printf("---------------------------\n");


    // --- 2. Fase de Inicio (Sin Regla de Intercambio) ---

    // TURNO 1: P1 (Humano)
    board_print(&board);
    printf("\n--- Turno de %c (Primer Movimiento) ---\n", p1.simbolo);
    int first_move_idx = p1.get_move(&board, p1.simbolo);
    actualizar_square(&board.squares[first_move_idx], p1.simbolo, p1.color);

    // TURNO 2: P2 (IA)
    board_print(&board);
    printf("\n--- Turno de %c (IA) ---\n", p2.simbolo);
    
    // Como la IA no intercambia, simplemente hace su primer movimiento
    printf("IA (%c) decide NO intercambiar.\n", p2.simbolo);
    int ai_first_move_idx = p2.get_move(&board, p2.simbolo);
    actualizar_square(&board.squares[ai_first_move_idx], p2.simbolo, p2.color);

    // El siguiente turno es de P1 (Humano)
    Player* current_player = &p1;
    printf("\n--- Comienza el juego regular ---\n");


    // --- 3. Bucle Principal del Juego ---
    while (1) {
        
        // Mostrar el tablero actual
        board_print(&board);
        printf("\n--- Turno de %c ---\n", current_player->simbolo);


        int move_indice = current_player->get_move(&board, current_player->simbolo);
        if (move_indice == -1) {
             printf("¡Tablero lleno! (Esto no debería pasar en Hex)\n");
             break;
        }

        actualizar_square(&board.squares[move_indice], current_player->simbolo, current_player->color);

        
        // --- Comprobar si hay Ganador ---


        char flat_board[MAX_BOARD_SIZE];
        for (int i = 0; i < board.size * board.size; i++) {
            flat_board[i] = board.squares[i].simbolo;
        }

        // 2. Comprobar SOLO al jugador actual
        int ha_ganado = 0;
        if (current_player->simbolo == p1.simbolo) { // p1 es 'X'
            ha_ganado = board_test_x(flat_board, board_size);
        } else { // El jugador actual es 'O'
            ha_ganado = board_test_o(flat_board, board_size);
        }
        
        // 3. Reaccionar si hubo un ganador
        if (ha_ganado) {
            printf("\n=========================\n");
            board_print(&board); // Mostrar el tablero final
            printf("¡¡¡ JUEGO TERMINADO !!!\n");
            
            // 'current_player' es quien acaba de ganar
            printf("   EL GANADOR ES: %c\n", current_player->simbolo); 
            printf("=========================\n");
            break; // Salir del bucle 'while(1)'
        }

        if (current_player->simbolo == p1.simbolo) {
            current_player = &p2;
        } else {
            current_player = &p1;
        }
    }

    board_destroy(&board);
    return 0;
}

//compile with: gcc -o hex_game main.c Board/board.c Board/square.c Board/Hex.c utils/utils.c Rules/temp.c -I./Board -I./utils -I./Rules
//run with: ./hex_game