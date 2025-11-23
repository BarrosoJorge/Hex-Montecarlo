#include "process_manager.h"
#include "../Player/player.h"
#include "pcg_basic.h"
#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_PROC 64
#define R 0
#define W 1

typedef struct {
    int fda[2]; // Child -> Parent
    int fdb[2]; // Parent -> Child
} pipe_t;

static pipe_t pipels[MAX_PROC];
static pid_t procls[MAX_PROC];
static int active_processes = 0;
static int64_t sims_per_process = 1000;



void child_loop(int id, int size_tablero_aprox) {
    char board[MAX_BOARD_SIZE];
    int64_t local_stats[MAX_BOARD_SIZE];
    // ELIMINADA: char player_info; (No se usaba)

    close(pipels[id].fda[R]);
    close(pipels[id].fdb[W]);

    pcg32_srandom(time(NULL) ^ getpid(), getpid());

    while(1) {
        ssize_t n = read(pipels[id].fdb[R], board, MAX_BOARD_SIZE);
        if (n <= 0) break; 
        if (board[0] == 0) break;

        int size = (int)board[MAX_BOARD_SIZE - 1];
        char player = board[MAX_BOARD_SIZE - 2];

        game_stats(board, size, player, sims_per_process, local_stats);

        // Casteo (void)! para silenciar warning de unused result
        if(write(pipels[id].fda[W], local_stats, sizeof(local_stats)) < 0) break;
    }

    close(pipels[id].fda[W]);
    close(pipels[id].fdb[R]);
    exit(0);
}

void processes_init(int num_processes, int64_t num_simulations) {
    if (num_processes > MAX_PROC) num_processes = MAX_PROC;
    if (num_processes < 1) num_processes = 1; // Seguridad
    active_processes = num_processes;
    sims_per_process = num_simulations/num_processes;

    for(int i = 0; i < active_processes; i++) {
        if(pipe(pipels[i].fda) == -1 || pipe(pipels[i].fdb) == -1) exit(1);
    }

    for(int i = 0; i < active_processes; i++) {
        procls[i] = fork();
        if (procls[i] < 0) exit(1);

        if (procls[i] == 0) {
            for(int j = 0; j < i; j++) {
                 close(pipels[j].fda[R]); close(pipels[j].fda[W]);
                 close(pipels[j].fdb[R]); close(pipels[j].fdb[W]);
            }
            child_loop(i, 0);
        }
    }

    for(int i = 0; i < active_processes; i++) {
        close(pipels[i].fda[W]);
        close(pipels[i].fdb[R]);
    }
}

int processes_get_best_move(const char* raw_board, int size, char player) {
    char buffer[MAX_BOARD_SIZE];
    for(int i=0; i<MAX_BOARD_SIZE; i++) buffer[i] = raw_board[i];
    
    buffer[MAX_BOARD_SIZE - 1] = (char)size;
    buffer[MAX_BOARD_SIZE - 2] = player;

    for(int i = 0; i < active_processes; i++) {
        // Silenciar warning
        if(write(pipels[i].fdb[W], buffer, MAX_BOARD_SIZE) < 0)perror("Error write padre");
    }

    int64_t total_stats[MAX_BOARD_SIZE];
    int64_t temp_stats[MAX_BOARD_SIZE];

    for(int i=0; i<MAX_BOARD_SIZE; i++) total_stats[i] = 0;

    for(int i = 0; i < active_processes; i++) {
        // Silenciar warning
        if(read(pipels[i].fda[R], temp_stats, sizeof(temp_stats)) > 0) {
            for(int k = 0; k < (size * size); k++) {
                total_stats[k] += temp_stats[k];
            }
        }
    }
    return game_move(total_stats, raw_board, size);
}

void processes_cleanup() {
    char kill_signal[MAX_BOARD_SIZE] = {0}; 
    for(int i = 0; i < active_processes; i++) {
        if(write(pipels[i].fdb[W], kill_signal, MAX_BOARD_SIZE)) {}; // Hack simple
    }
    for(int i = 0; i < active_processes; i++) {
        waitpid(procls[i], NULL, 0);
        close(pipels[i].fda[R]);
        close(pipels[i].fdb[W]);
    }
}