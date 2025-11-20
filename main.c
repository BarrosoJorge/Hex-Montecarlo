#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // Para atoi, atoll
#include "board.h"
#include "player.h"
#include "rules.h"
#include "Processes/process_manager.h" // Módulo que gestiona la paralelizacion

//Funciones de Ayuda 

void print_help() {
    printf("Uso: ./hex_game <tamano_tablero> <num_procesos> <num_simulaciones>\n");
    printf("  tamano_tablero:   Tamano del lado del tablero (ej. 7, 11, 14).\n");
    printf("  num_procesos:     Numero de procesos hijos (workers) para la IA (ej. 4, 8).\n");
    printf("  num_simulaciones: Simulaciones por proceso (ej. 1000, 10000).\n");
    exit(1);
}

void error_msg(const char* msg) {
    fprintf(stderr, "Error: %s\n", msg);
    exit(2);
}

//  Main 

int main(int argc, char** argv) {
    //  0. Validación y Parseo de Argumentos 
    if (argc != 4) {
        print_help();
    }

    int board_size = atoi(argv[1]);
    if (board_size < 3 || board_size > MAX_BOARD_SIDE) {
        error_msg("Tamano de tablero invalido (debe ser entre 3 y 26).");
    }

    int num_procs = atoi(argv[2]);
    if (num_procs < 1) {
        error_msg("El numero de procesos debe ser al menos 1.");
    }

    int64_t num_sims = atoll(argv[3]); // atoll para leer enteros largos (long long)
    if (num_sims < 1) {
        error_msg("El numero de simulaciones debe ser positivo.");
    }

    printf("Configuración: Tablero %dx%d | Procesos: %d | Sims/Proc: %ld\n", 
            board_size, board_size, num_procs, num_sims);


    //  1. Inicialización del Sistema 
    
    // INICIALIZAR PROCESOS: Crea los hijos y los pipes antes de que empiece el juego.
    // Los hijos se quedarán en un bucle infinito esperando órdenes.
    processes_init(num_procs, num_sims);

    Board board;
    board_init(&board, board_size);

    // Inicializar Jugadores
    // P1: Humano
    // P2: IA (Internamente, get_ai_move_montecarlo usará el Process Manager)
    Player p1 = {'X', 1, get_human_move}; 
    Player p2 = {'O', 2, get_ai_move_montecarlo};

    printf("--- ¡Bienvenido a Hex Paralelo! ---\n");
    printf("Jugador 1: %c (Humano)\n", p1.simbolo);
    printf("Jugador 2: %c (IA Montecarlo)\n", p2.simbolo);
    printf("---------------------------\n");


    //  2. Fase de Inicio (Primeros Movimientos) 

    // TURNO 1: P1 (Humano)
    board_print(&board);
    printf("\n--- Turno de %c (Primer Movimiento) ---\n", p1.simbolo);
    int first_move_idx = p1.get_move(&board, p1.simbolo);
    actualizar_square(&board.squares[first_move_idx], p1.simbolo, p1.color);

    // TURNO 2: P2 (IA)
    board_print(&board);
    printf("\n--- Turno de %c (IA) ---\n", p2.simbolo);
    
    printf("IA (%c) decide NO intercambiar (Lógica simplificada).\n", p2.simbolo);
    // Aquí la IA "piensa" usando los procesos que inicializamos arriba
    int ai_first_move_idx = p2.get_move(&board, p2.simbolo);
    actualizar_square(&board.squares[ai_first_move_idx], p2.simbolo, p2.color);

    // El siguiente turno es de P1 (Humano)
    Player* current_player = &p1;
    printf("\n--- Comienza el juego regular ---\n");


    //  3. Bucle Principal del Juego 
    while (1) {
        
        // Mostrar el tablero actual
        board_print(&board);
        printf("\n--- Turno de %c ---\n", current_player->simbolo);

        // Obtener movimiento
        // Si es humano: pide input.
        // Si es IA: delega a process_manager -> workers -> pipes.
        int move_indice = current_player->get_move(&board, current_player->simbolo);
        
        if (move_indice == -1) {
             printf("¡Tablero lleno! (Esto no debería pasar en Hex)\n");
             break;
        }

        actualizar_square(&board.squares[move_indice], current_player->simbolo, current_player->color);

        
        //  Comprobación de Ganador (Optimizada) 
        
        // 1. Convertir a flat_board para rules.c
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
            printf("   EL GANADOR ES: %c\n", current_player->simbolo); 
            printf("=========================\n");
            break; // Salir del bucle
        }

        //  Cambiar de Turno 
        if (current_player->simbolo == p1.simbolo) {
            current_player = &p2;
        } else {
            current_player = &p1;
        }
    }

    //  4. Limpieza de Recursos 
    printf("Limpiando procesos y memoria...\n");
    
    // Envía la señal de terminación a los procesos hijos y espera a que mueran.
    processes_cleanup(); 
    
    board_destroy(&board);
    
    return 0;
}

// gcc -o hex_game main.c Board/board.c Board/square.c utils/utils.c utils/pcg_basic.c Player/player.c Rules/rules.c Processes/process_manager.c -I./Board -I./utils -I./Player -I./Rules -I./Processes -lm

// ./hex_game 7 4 10000