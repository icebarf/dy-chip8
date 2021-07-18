#pragma once
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Display Pixel defines */
#define TRUE ON
#define FALSE OFF

typedef struct chip8_t {
    bool display[64][32]; /* Display */
    uint8_t memory[4096]; /* 4 kb memory */
    uint8_t *PC;          /* Program counter, points at next instruction */
    uint16_t *index; /* Index register, points at any instruction in memory */
    uint16_t stack[16];    /* Stack to hold 16 - 2 Byte instructions */
    uint8_t delay_timer;   /* Delay timer */
    uint8_t sound_timer;   /* Sound timer */
    uint8_t registers[16]; /* 16, 8-bit general purpose
                            registers from V0 to VF */
} chip8_t;
