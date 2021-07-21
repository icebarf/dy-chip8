#pragma once
#include "chip8.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include <sys/types.h>

int create_window(SDL_Window *screen, SDL_Renderer *renderer,
                  SDL_Texture *texture);

void destroy_window(SDL_Window *screen, SDL_Renderer *renderer,
                    SDL_Texture *texture);

void draw_to_window(SDL_Renderer *renderer, SDL_Texture *texture,
                    uint32_t *pixels);