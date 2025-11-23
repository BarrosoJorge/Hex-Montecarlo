#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "board.h"
#include "player.h"
#include "rules.h"
#include "Processes/process_manager.h"
#include "Gui/gui.h"
#include "utils.h"

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

// Wrapper solo para mostrar estado IA
void get_human_move_gui_wrapper_void(const Board* board, char player_symbol) {
    char msg[64];
    sprintf(msg, "Turno de %c (IA) Pensando...", player_symbol);
    gui_set_status(msg);
}

int get_human_move_gui_wrapper(const Board* board, char player_symbol) {
    char msg[64];
    sprintf(msg, "Turno de %c (RED) - Tu Turno", player_symbol);
    gui_set_status(msg);
    return gui_get_human_move(board);
}

int main(int argc, char** argv) {
    GameConfig config;
    // Valores por defecto
    config.board_size = 7;
    config.num_procs = 20;
    config.num_sims = 1000000; 
    config.player_color = 'X';

    if (argc >= 4) {
        config.board_size = atoi(argv[1]);
        config.num_procs = atoi(argv[2]); 
        config.num_sims = atoi(argv[3]);
        printf("Configuracion por Argumentos: Tamano=%d, Procesos=%d, Sims=%ld\n", 
               config.board_size, config.num_procs, config.num_sims);
    } 
    else {
        
        printf("Argumentos insuficientes. Usando valores por defecto.\n");
        printf("Configuracion por defecto: Tamano=%d, Procesos=%d, Sims=%ld\n", 
               config.board_size, config.num_procs, config.num_sims);
    }


    gui_init("Hex Game - Proyecto Paralelo");

    while (1) {
        int option = gui_main_menu();
        
        if (option == 0) break; // SALIR
        if (option == 2) {
            gui_config_screen(&config);
            continue;
        }

        if (option == 1) { // JUGAR
            // Iniciar procesos
            processes_init(config.num_procs, config.num_sims);
            
            int replay = 1;
            while(replay) { // Bucle de reinicio
                Board board;
                // IMPORTANTE: Aquí se usa el tamaño seleccionado en config
                board_init(&board, config.board_size);
                
                Player p1 = {'X', 1, get_human_move_gui_wrapper};
                Player p2 = {'O', 2, get_ai_move_montecarlo};
                
                Player* current = &p1;
                int running = 1;

                while (running) {
                    gui_draw(&board);

                    char status[128];
                    if (current->simbolo == 'X') {
                        sprintf(status, "Turno RED (Humano)");
                    } else {
                        sprintf(status, "Turno BLUE (IA: %ld sims)", config.num_sims);
                        gui_process_events(); 
                    }
                    gui_set_status(status);

                    int move = current->get_move(&board, current->simbolo);
                    
                    if (move == -1) { 
                        running = 0; 
                        replay = 0; 
                        break; 
                    }

                    actualizar_square(&board.squares[move], current->simbolo, current->color);
                    
                    // Verificar Ganador
                    char flat[MAX_BOARD_SIZE];
                    convert_board_to_char(&board, flat);
                    
                    int winner = 0;
                    if (current->simbolo == 'X') winner = board_test_x(flat, config.board_size);
                    else winner = board_test_o(flat, config.board_size);

                    if (winner) {
                        gui_draw(&board); 
                        replay = gui_game_over_menu(&board, current->simbolo == 'X' ? "RED" : "BLUE");
                        running = 0; 
                    }
                    current = (current->simbolo == p1.simbolo) ? &p2 : &p1;
                }
                board_destroy(&board);
            }
            processes_cleanup();
        }
    }
    gui_close();
    return 0;
}