#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // Para atoi, atoll
#include "board.h"
#include "player.h"
#include "rules.h"
#include "Processes/process_manager.h"
#include "Gui/gui.h" // <--- INCLUIMOS EL MÓDULO GRÁFICO

// --- Funciones de Ayuda ---

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

// --- Wrapper para conectar Player con GUI ---
// Esta función adapta la firma de 'get_move_callback_t' a 'gui_get_human_move'
int get_human_move_gui_wrapper(const Board* board, char player_symbol) {
    // Actualizamos el título de la ventana para avisar al jugador
    char msg[64];
    sprintf(msg, "Turno de %c (Tu Turno) - Haz click en una casilla", player_symbol);
    gui_set_status(msg);
    
    // Llamamos a la función gráfica que espera el click
    return gui_get_human_move(board);
}

// --- Main ---

int main(int argc, char** argv) {
    // --- 0. Validación y Parseo de Argumentos ---
    if (argc != 4) print_help();

    int board_size = atoi(argv[1]);
    if (board_size < 3 || board_size > MAX_BOARD_SIDE) error_msg("Tamano invalido.");

    int num_procs = atoi(argv[2]);
    if (num_procs < 1) error_msg("Procesos debe ser >= 1.");

    int64_t num_sims = atoll(argv[3]);
    if (num_sims < 1) error_msg("Simulaciones debe ser positivo.");

    printf("Config: Tablero %d | Proc %d | Sims %ld\n", board_size, num_procs, num_sims);

    // --- 1. Inicialización ---
    
    // Iniciar Procesos (Backend)
    processes_init(num_procs, num_sims);

    // Iniciar Gráficos (Frontend)
    gui_init(0, "Hex Game - Montecarlo Paralelo");

    Board board;
    board_init(&board, board_size);

    // Configurar Jugadores
    // P1 usa el wrapper gráfico, P2 usa la IA Montecarlo
    Player p1 = {'X', 1, get_human_move_gui_wrapper}; 
    Player p2 = {'O', 2, get_ai_move_montecarlo};

    // --- 2. Fase de Inicio ---

    // Turno P1 (Humano)
    gui_draw(&board);
    int first_move = p1.get_move(&board, p1.simbolo);
    if (first_move == -1) goto cleanup; // Si cerró la ventana
    actualizar_square(&board.squares[first_move], p1.simbolo, p1.color);

    // Turno P2 (IA)
    gui_draw(&board);
    gui_set_status("Turno de O (IA Pensando...)");
    gui_process_events(); // Refrescar ventana antes de bloquear
    
    int ai_move = p2.get_move(&board, p2.simbolo);
    actualizar_square(&board.squares[ai_move], p2.simbolo, p2.color);

    Player* current_player = &p1;

    // --- 3. Bucle Principal ---
    while (1) {
        // Actualizar Gráficos
        gui_draw(&board);

        // Actualizar Estado/Título
        char status[128];
        if (current_player->simbolo == 'X') {
            sprintf(status, "Turno de %c (Humano)", current_player->simbolo);
        } else {
            sprintf(status, "Turno de %c (IA Calculando %ld sims...)", current_player->simbolo, num_sims * num_procs);
            // Importante: Procesar eventos para que la ventana se pinte antes de que la IA bloquee el proceso
            gui_process_events();
        }
        gui_set_status(status);

        // Obtener Movimiento
        int move_idx = current_player->get_move(&board, current_player->simbolo);

        // Si devuelve -1, significa que se cerró la ventana o error
        if (move_idx == -1) break;

        // Aplicar Movimiento
        actualizar_square(&board.squares[move_idx], current_player->simbolo, current_player->color);
        gui_draw(&board); // Redibujar inmediatamente

        // Checar Ganador
        char flat_board[MAX_BOARD_SIZE];
        convert_board_to_char(&board, flat_board); // Usamos la utilidad que ya creamos

        int ha_ganado = 0;
        if (current_player->simbolo == 'X') ha_ganado = board_test_x(flat_board, board_size);
        else ha_ganado = board_test_o(flat_board, board_size);

        if (ha_ganado) {
            gui_draw(&board);
            char win_msg[64];
            sprintf(win_msg, "¡GANADOR: JUGADOR %c! (Cierra la ventana para salir)", current_player->simbolo);
            gui_set_status(win_msg);
            printf("\n>>> JUEGO TERMINADO. GANADOR: %c <<<\n", current_player->simbolo);
            
            // Esperar a que el usuario cierre la ventana
            while (gui_get_human_move(&board) != -1) {
                // Bucle infinito visual hasta que cierren la app
            }
            break;
        }

        // Cambiar Turno
        current_player = (current_player->simbolo == p1.simbolo) ? &p2 : &p1;
    }

cleanup:
    // --- 4. Limpieza ---
    printf("Cerrando recursos...\n");
    gui_close();
    processes_cleanup();
    board_destroy(&board);

    return 0;
}

// gcc -o hex_game main.c Board/board.c Board/square.c utils/utils.c utils/pcg_basic.c Player/player.c Rules/rules.c Processes/process_manager.c -I./Board -I./utils -I./Player -I./Rules -I./Processes -lm

// ./hex_game 7 4 10000