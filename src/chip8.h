#pragma once
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/* Display Pixel defines */
#define TRUE ON
#define FALSE OFF

/* Window defines */
#define WIN_W 32
#define WIN_H 64

/* Fontset characters */
#define FONTCHARS 0x0050

/* Push and Pop for stack */
#define Pop(x) x = chip8.stack[top--]
#define Push(x) chip8.stack[++top] = x

typedef struct chip8_t {
    bool display[WIN_H][WIN_W]; /* Display */
    uint8_t memory[4096];       /* 4 kb memory */
    uint16_t PC;    /* Program counter, points at next instruction */
    uint16_t index; /* Index register, points at any instruction in memory */
    uint16_t stack[16];    /* Stack to hold 16 - 2 Byte instructions */
    uint8_t delay_timer;   /* Delay timer */
    uint8_t sound_timer;   /* Sound timer */
    uint8_t registers[16]; /* 16, 8-bit general purpose
                            registers from V0 to VF */
} chip8_t;