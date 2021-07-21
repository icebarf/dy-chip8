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
#define WIN_W 64
#define WIN_H 32

/* Chip-8 Defines */
#define KEY_S 16
#define FONT_S 16
#define MEMORY_S 4096
#define STACK_S 16
#define REGISTERS_S 16

/* Fontset characters */
#define FONTCHARS 0x0050

/* Push and Pop for stack */
#define Pop(x) x = chip8.stack[top--]
#define Push(x) chip8.stack[++top] = x

typedef struct chip8_t {
    uint8_t display[WIN_W * WIN_H];
    uint8_t draw;
    uint32_t pixels[2048];
    uint8_t keyboard[KEY_S];

    uint8_t key_states[KEY_S];
    uint8_t current_key;

    uint8_t fontset[FONT_S];

    uint8_t memory[MEMORY_S];
    uint16_t PC;
    uint16_t index;

    uint16_t stack[STACK_S];

    uint8_t registers[REGISTERS_S];

    uint8_t delay_timer;
    uint8_t sound_timer;
} chip8_t;

/* Enumaerated Type for accessing keyboard */
enum keys {
    /* 1 2 3 C */
    one_k,
    two_k,
    three_k,
    C_k,
    /* 4 5 6 D */
    four_k,
    five_k,
    six_k,
    D_k,
    /* 7 8 9 E */
    seven_k,
    eight_k,
    nine_k,
    E_k,
    /* A B 0 F */
    A_k,
    zero_k,
    B_k,
    F_k
};