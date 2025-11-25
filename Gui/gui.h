#ifndef GUI_H
#define GUI_H

#include "board.h"
#include <stdint.h> 

typedef struct {
    int board_size;    // Variable para el tamaño
    int64_t num_sims;  // Variable para dificultad
    char player_color; 
    int num_procs;
} GameConfig;

/*
Inicializa SDL, la ventana y el renderer.
    title: Título de la ventana
*/
void gui_init(const char* title);

/*
Cierra SDL y libera memoria gráfica.
*/
void gui_close();

/*
Dibuja el estado actual del tablero y actualiza la pantalla.
    board: puntero al tablero
*/
void gui_draw(const Board* board);

// Menús


/*
Muestra el menú principal (Jugar, Configurar, Salir).
Retorna: 0=Salir, 1=Jugar, 2=Configurar
*/
int gui_main_menu();

/*
Muestra la pantalla de configuración para modificar la dificultad y tamaño.
Modifica directamente la estructura config pasada por referencia.
*/
void gui_config_screen(GameConfig* config);

/*
Muestra una ventana flotante sobre el tablero con el ganador.
Permite arrastrar la ventana para ver el juego finalizado.
Retorna: 1=Jugar de nuevo, 0=Ir al menú
*/
int gui_game_over_menu(const Board* board, const char* winner);

// Juego

/*
Actualiza el título de la ventana con información de estado (ej. "Pensando...").
*/
void gui_set_status(const char* status);

/*
Espera y procesa un clic del usuario en una casilla válida.
Retorna el índice de la casilla seleccionada.
*/
int gui_get_human_move(const Board* board);

/*
Procesa eventos de SDL (como cerrar ventana) sin bloquear la ejecución.
Útil para llamar mientras la IA piensa.
*/
void gui_process_events();

#endif