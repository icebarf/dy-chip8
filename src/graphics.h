#pragma once
#include "chip8.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>

int create_window();

void destroy_window(SDL_Window *window, SDL_Renderer *renderer);

void draw_to_window(uint8_t x, uint8_t y, uint8_t n);