#pragma once
#include "chip8.h"

#if defined(_POSIX_VERSION) || defined(__linux__) || defined(TARGET_OS_MAC)
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#endif

#if defined(_WIN32)
#include "SDL2/SDL.h"
#endif

#include <sys/types.h>

int create_window();

void destroy_window();

void draw_to_window(uint32_t *pixels);
