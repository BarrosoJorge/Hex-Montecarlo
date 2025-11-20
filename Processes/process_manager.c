#include "process_manager.h"
#include "../Player/player.h" // Necesitamos game_stats y game_move
#include "pcg_basic.h"     // Para inicializar RNG en hijos
#include "board.h"   // Para MAX_BOARD_SIZE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_PROC 64
#define R 0
#define W 1

typedef struct {
    int fda[2]; // Pipe Child -> Parent (Resultados)
    int fdb[2]; // Pipe Parent -> Child (Tablero)
} pipe_t;

static pipe_t pipels[MAX_PROC];
static pid_t procls[MAX_PROC];
static int active_processes = 0;
static int64_t sims_per_process = 1000;

void child_loop(int id, int size_tablero_aprox) {
    char board[MAX_BOARD_SIZE];
    int64_t local_stats[MAX_BOARD_SIZE];
    char player_info; // Podríamos necesitar enviar quién juega, por ahora inferimos o enviamos extra

    // Cerrar extremos innecesarios del propio pipe
    close(pipels[id].fda[R]);
    close(pipels[id].fdb[W]);

    // Inicializar RNG único para este hijo
    pcg32_srandom(time(NULL) ^ getpid(), getpid());

    while(1) {
        // 1. Leer Tablero del Padre
        // Leemos un tamaño fijo máximo por simplicidad y velocidad
        int n = read(pipels[id].fdb[R], board, MAX_BOARD_SIZE);
        if (n <= 0) break; // Pipe roto o fin

        // Protocolo de salida: Si board[0] es 0 (null char), salimos
        if (board[0] == 0) break;

        // Detectar tamaño real (lo pasaremos o inferiremos)
        // Por simplicidad del protocolo actual, asumimos que el board contiene la info necesaria
        // OJO: Necesitamos saber el 'size' y el 'player' para game_stats.
        // HACK DE OPTIMIZACIÓN:
        // Vamos a asumir que el byte MAX_BOARD_SIZE-1 tiene el 'size' 
        // y MAX_BOARD_SIZE-2 tiene el 'player'.
        int size = (int)board[MAX_BOARD_SIZE - 1];
        char player = board[MAX_BOARD_SIZE - 2];

        // 2. Calcular Estadísticas (Heavy Lifting)
        // game_stats está en player.c
        game_stats(board, size, player, sims_per_process, local_stats);

        // 3. Enviar Resultados al Padre
        write(pipels[id].fda[W], local_stats, sizeof(local_stats));
    }

    // Limpieza final del hijo
    close(pipels[id].fda[W]);
    close(pipels[id].fdb[R]);
    exit(0);
}


void processes_init(int num_processes, int64_t num_simulations) {
    if (num_processes > MAX_PROC) num_processes = MAX_PROC;
    active_processes = num_processes;
    sims_per_process = num_simulations;

    // Crear Pipes
    for(int i = 0; i < active_processes; i++) {
        if(pipe(pipels[i].fda) == -1 || pipe(pipels[i].fdb) == -1) {
            perror("Error creando pipes");
            exit(1);
        }
    }

    // Crear Procesos
    for(int i = 0; i < active_processes; i++) {
        procls[i] = fork();
        
        if (procls[i] < 0) {
            perror("Error en fork");
            exit(1);
        }

        if (procls[i] == 0) {
            // Soy el HIJO
            
            // Cerrar pipes de OTROS hijos (importante para no dejar FDs abiertos)
            for(int j = 0; j < i; j++) { // Solo los creados anteriormente
                 close(pipels[j].fda[R]); close(pipels[j].fda[W]);
                 close(pipels[j].fdb[R]); close(pipels[j].fdb[W]);
            }
            // El resto de pipes futuros no existen aún para este proceso
            
            child_loop(i, 0); // Nunca retorna
        }
    }

    // Soy el PADRE: Cerrar extremos que usa el hijo
    for(int i = 0; i < active_processes; i++) {
        close(pipels[i].fda[W]); // El padre solo lee de A
        close(pipels[i].fdb[R]); // El padre solo escribe en B
    }
}

int processes_get_best_move(const char* raw_board, int size, char player) {
    // 1. Preparar el payload (Tablero + Metadata)
    char buffer[MAX_BOARD_SIZE];
    // Copiamos el tablero
    for(int i=0; i<MAX_BOARD_SIZE; i++) buffer[i] = raw_board[i];
    
    // HACK: Inyectamos metadata al final del buffer para que el hijo sepa qué hacer
    // Asegúrate que MAX_BOARD_SIZE sea suficiente (400+) y el tablero real (7x7=49) no lo solape.
    buffer[MAX_BOARD_SIZE - 1] = (char)size;
    buffer[MAX_BOARD_SIZE - 2] = player;

    // 2. Enviar trabajo a TODOS los hijos
    for(int i = 0; i < active_processes; i++) {
        write(pipels[i].fdb[W], buffer, MAX_BOARD_SIZE);
    }

    // 3. Recibir y agregar resultados
    int64_t total_stats[MAX_BOARD_SIZE];
    int64_t temp_stats[MAX_BOARD_SIZE];

    // Limpiar acumulador
    for(int i=0; i<MAX_BOARD_SIZE; i++) total_stats[i] = 0;

    for(int i = 0; i < active_processes; i++) {
        read(pipels[i].fda[R], temp_stats, sizeof(temp_stats));
        
        // Sumar al total
        for(int k = 0; k < (size * size); k++) {
            total_stats[k] += temp_stats[k];
        }
    }

    // 4. Elegir mejor movimiento
    return game_move(total_stats, raw_board, size);
}

void processes_cleanup() {
    char kill_signal[MAX_BOARD_SIZE] = {0}; // Primer byte 0 mata al hijo

    // Enviar señal de muerte
    for(int i = 0; i < active_processes; i++) {
        write(pipels[i].fdb[W], kill_signal, MAX_BOARD_SIZE);
    }

    // Esperar a que mueran (evitar zombies)
    for(int i = 0; i < active_processes; i++) {
        waitpid(procls[i], NULL, 0);
        close(pipels[i].fda[R]);
        close(pipels[i].fdb[W]);
    }
}