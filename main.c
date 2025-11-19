#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

// Includes locales
#include "pcg_basic.h"
#include "Board/board.h"
#include "Board/square.h"
#include "Player/player.h"
#include "Rules/rules.h"
#include "Utils/utils.h"

#define MAX_PROC 32
#define BUFLEN 128
#define R 0
#define W 1

typedef struct {
    int fda[2];
    int fdb[2];
} pipe_t;

void print_help() {
    printf("Usage: hex <nproc> <nsim> <player> <bsize>\n");
    printf("nproc: Number of processes to run.\n");
    printf("nsim: Number of simulations per hex.\n");
    printf("player: Can be \'X\' or \'O\'\n");
    exit(1);
}

void error_msg(const char* msg) {
    fprintf(stderr, "Error: %s\n", msg);
    exit(2);
}

int main(int argc, char** argv) {
    if(argc != 5) print_help();

    int nproc = atoi(argv[1]);
    if(nproc < 1 || nproc > MAX_PROC) {
        error_msg("Wrong number of processes");
    }

    int64_t nsim = atoll(argv[2]);
    if(nsim < 1) error_msg("Wrong number of simulations");

    char player = argv[3][0];
    if(player != 'O' && player != 'X') error_msg("Wrong player");
    
    // El oponente es quien no eres tú
    char oponent = (player == 'X') ? 'O' : 'X';

    int size = atoi(argv[4]);
    if(size < 3 || size > MAX_BOARD_SIDE) error_msg("Wrong board size");

    // PRNG INITIALIZATION
    pcg32_srandom(time(0), getpid());
    
    int64_t stats[MAX_BOARD_SIZE];
    int64_t stmp[MAX_BOARD_SIZE];
    char board[MAX_BOARD_SIZE];

    // PIPES
    pipe_t pipels[MAX_PROC];
    for(int i = 0; i < nproc; i++) {
        if(pipe(pipels[i].fda) == -1 || pipe(pipels[i].fdb) == -1)
             error_msg("Failed to create pipes");
    }

    // PROCESSES
    pid_t procls[MAX_PROC];
    int rank = 0;
    for(int i = 0; i < nproc; i++) {
        procls[i] = fork();
        if (procls[i] < 0) error_msg("Fork failed");
        
        // Child
        if(procls[i] == 0) {
            rank = i + 1;
            break;
        }
    }

    // PROCESO PADRE
    if(rank == 0) {
        // Configuracion de pipes
        for(int i = 0; i < nproc; i++) {
            close(pipels[i].fda[W]);
            close(pipels[i].fdb[R]);
        }

        board_clean(board, size);
        board_print(board, size);
        char buffer[BUFLEN];
        int pos;
        
        while(1) {
            printf("%c: ", player);
            fflush(stdout);
            
            if (!fgets(buffer, BUFLEN, stdin)) break;
            trim(buffer);

            if(strcmp(buffer, "quit") == 0) break;

            pos = read_move(buffer, size);
            if(pos < 0) {
                printf("Movimiento inválido.\n");
                continue;
            }
            if(board[pos] != '+') {
                printf("Casilla ocupada.\n");
                continue;
            }

            // Juego (Movimiento del Usuario)
            board[pos] = player;
            board_print(board, size);
            if(board_test(board, size) == player) {
                printf("Gana %c!\n", player);
                break;
            }

            // SIMULACIÓN PARALELA (Turno IA/Oponente)
            printf("Pensando...\n");
            
            // Write Board --> Children
            for(int i = 0; i < nproc; i++) {
                write(pipels[i].fdb[W], board, sizeof(board));
            }
            
            // Read Stats <-- Children
            for(int i = 0; i < MAX_BOARD_SIZE; i++) stats[i] = 0;
            for(int i = 0; i < nproc; i++) {
                read(pipels[i].fda[R], stmp, sizeof(stmp));
                for(int k = 0; k < (size * size); k++) stats[k] += stmp[k];
            }    

            // Decisión de la IA
            pos = game_move(stats, board, size);
            if (pos != -1) {
                board[pos] = oponent;
                printf("IA juega en: ");
                int px, py;
                to_xy(size, pos, &px, &py);
                printf("%c%d\n", 'A' + px, py + 1);
                
                board_print(board, size);
                if(board_test(board, size) == oponent) {
                    printf("Gana %c (IA)!\n", oponent);
                    break;
                }
            } else {
                printf("Empate o tablero lleno.\n");
                break;
            }
        }

        // Finalizacion (Matar hijos suavemente)
        board[0] = 0; // Señal de fin
        for(int i = 0; i < nproc; i++) {
            write(pipels[i].fdb[W], board, sizeof(board));
        }

        for(int i = 0; i < nproc; i++) {
            waitpid(procls[i], NULL, 0);
        }

        for(int i = 0; i < nproc; i++) {
            close(pipels[i].fda[R]);
            close(pipels[i].fdb[W]);
        }
    }

    // PROCESOS HIJOS (Workers)
    if(rank > 0) {
        // Configuracion de los pipes
        for(int i = 0; i < nproc; i++) {
            if(i == rank - 1) continue;
            close(pipels[i].fda[R]);
            close(pipels[i].fda[W]);
            close(pipels[i].fdb[R]);
            close(pipels[i].fdb[W]);
        }
        // Cerrar extremos no usados del pipe propio
        close(pipels[rank - 1].fda[R]); 
        close(pipels[rank - 1].fdb[W]); 

        while(1) {
            int nread = read(pipels[rank - 1].fdb[R], board, sizeof(board));
            if (nread <= 0) break; // Pipe roto o EOF

            if(board[0] == 0) {
                close(pipels[rank - 1].fda[W]);
                close(pipels[rank - 1].fdb[R]);
                break;
            }

            // El proceso hijo calcula simulaciones para el oponente (la IA)
            game_stats(board, size, oponent, nsim, stmp); // Usamos stmp local
            write(pipels[rank - 1].fda[W], stmp, sizeof(stmp));
        }
    }

    return 0;
}