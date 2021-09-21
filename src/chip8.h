#pragma once

#if defined(_POSIX_VERSION) || defined(__linux__) || defined(TARGET_OS_MAC)
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#endif

#if defined(_WIN32)
#include "SDL2/SDL.h"
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>


/* Display Pixel defines */
#define TRUE 1
#define FALSE 0

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
    uint8_t beep;
} chip8_t;

/* Enumaerated Type for accessing keyboard */
enum keys {
    /* 1 2 3 C */
    zero_k,
    one_k,
    two_k,
    three_k,
    four_k,
    five_k,
    six_k,
    seven_k,
    eight_k,
    nine_k,
    A_k,
    B_k,
    C_k,
    D_k,
    E_k,
    F_k
};
