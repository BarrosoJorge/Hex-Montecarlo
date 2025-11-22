#ifndef GUI_H
#define GUI_H

#include "../Board/board.h"
#include <stdint.h> 

typedef struct {
    int board_size;    // Variable para el tamaño
    int64_t num_sims;  // Variable para dificultad
    char player_color; 
    int num_procs;
} GameConfig;

void gui_init(const char* title);
void gui_close();
void gui_draw(const Board* board);

// Menús
int gui_main_menu();
void gui_config_screen(GameConfig* config);
int gui_game_over_menu(const char* winner);

// Juego
void gui_set_status(const char* status);
int gui_get_human_move(const Board* board);
void gui_process_events();

#endif