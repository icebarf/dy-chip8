#pragma once
#include "chip8.h"

extern chip8_t chip8;
extern uint16_t opcode_main;
extern int read_size;
extern int top;


void decode_and_execute();
