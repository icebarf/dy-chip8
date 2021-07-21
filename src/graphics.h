#pragma once
#include "chip8.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include <sys/types.h>

int create_window();

void destroy_window();

void draw_to_window(uint32_t *pixels);