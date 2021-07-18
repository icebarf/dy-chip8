#include "chip8.h"

extern chip8_t chip8;

void fetch(int read_size) {
    for (int i = 0; i < read_size; i++) {
        /* Big endian 16-bit instruction created from two 8 bit little endian
         * instructions */
        int opcode = chip8.memory[512 + i] | chip8.memory[512 + i + 1];
    }
}