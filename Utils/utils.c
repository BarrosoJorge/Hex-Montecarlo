#include "utils.h"

void trim(char* str) {
    int i = 0;
    while(str[i] != '\0') {
        if(str[i] == '\n' || str[i] == '\r') {
            str[i] = '\0';
            break;
        }
        i++;
    }
}

int read_move(const char* buffer, int size) {
    int r, c;

    if(buffer[0] >= 'A' && buffer[0] <= 'Z') {
        c = buffer[0] - 'A';
    }
    else if(buffer[0] >= 'a' && buffer[0] <= 'z') {
        c = buffer[0] - 'a';
    }
    else return -1;

    r = atoi(buffer + 1) - 1;

    return to_ind(size, c, r);
}