#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <stdint.h> // Para int64_t

/**
 * Inicializa el pool de procesos hijos y los pipes.
 * Los hijos entrarán inmediatamente en un bucle de espera.
 * num_processes Número de procesos hijos a crear.
 * num_simulations Número de simulaciones por proceso.
 */
void processes_init(int num_processes, int64_t num_simulations);

/**
 * Envía el tablero a los hijos, espera sus cálculos y devuelve el mejor movimiento.
 * raw_board Array de chars del tablero (formato rápido).
 * size Tamaño del tablero.
 * player Turno actual ('X' o 'O').
 *  El índice del mejor movimiento.
 */
int processes_get_best_move(const char* raw_board, int size, char player);

/**
 * Manda señal de terminación a los hijos y limpia recursos (pipes/pids).
 */
void processes_cleanup();

#endif