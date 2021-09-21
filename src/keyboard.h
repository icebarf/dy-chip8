#pragma once
#include "chip8.h"
#include "graphics.h"

#if defined(_POSIX_VERSION) || defined(__linux__) || defined(TARGET_OS_MAC)
#include <SDL2/SDL_events.h>
#endif

#if defined(_WIN32)
#include "SDL2/SDL_events.h"
#endif

void handle_input(int state, SDL_Event *event);
