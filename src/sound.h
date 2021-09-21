#pragma once
#include "chip8.h"
#include "graphics.h"


#if defined(_POSIX_VERSION) || defined(__linux__) || defined(TARGET_OS_MAC)
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#endif

#if defined(_WIN32)
#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL.h"
#endif

void init_sound(char *sound_path);
void play_sound();
void destroy_sound();
