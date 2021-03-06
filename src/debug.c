#include "debug.h"

extern chip8_t chip8;

void print_memory() {
    for (int i = 0; i < 4096; i += 16) {
        printf("0x%04x - 0x%04x    ", i, i + 15);
        for (int j = 0; j < 16; j++) {
            printf("%02x  ", chip8.memory[i + j]);
        }
        puts("");
    }
    puts("");
    puts("======================================================"
         "================");
    puts("");
}

void log_to_terminal() {
    time_t now = time(&now);
    printf("Emu Log - %s", ctime(&now));
    puts("");
    puts("======================================================="
         "================");
    puts("");
}

void print_screen() {
    puts("");
    puts("======================================================="
         "================");
    puts("");
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 64; j++) {
            if (chip8.display[i * 64 + j] == 1)
                printf("*");
            else
                printf("-");
        }
        printf("\n");
    }
}