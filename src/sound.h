#pragma once
#include "chip8.h"
#include "graphics.h"
#include <SDL2/SDL_mixer.h>

void init_sound(char *alarm);
void play_sound();
void destroy_sound();