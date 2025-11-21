#ifndef GUI_H
#define GUI_H

#include "../Board/board.h"

// Inicializa la ventana de SDL y el renderizador
void gui_init(int board_size_pixels, const char* title);

// Limpia memoria y cierra SDL
void gui_close();

// Dibuja el estado actual del tablero
void gui_draw(const Board* board);

// Muestra un mensaje (para ganador o turno) en el título de la ventana
void gui_set_status(const char* status);

// Espera a que el humano haga clic en una casilla válida.
// Mantiene la ventana respondiendo mientras espera.
// Retorna: El índice del movimiento (0 a size*size-1) o -1 si cierra la ventana.
int gui_get_human_move(const Board* board);

// Procesa eventos de ventana (para que no se congele mientras la IA piensa)
void gui_process_events();

#endif