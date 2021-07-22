#include "sound.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>

Mix_Chunk *beep = NULL;

void init_sound(char *alarm) {
    /* Initialise Mixer */
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("Unable to initialise sound: %s", SDL_GetError());
        exit(1);
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Unable to initialise SDL Mixer: %s", Mix_GetError());
        exit(1);
    }

    beep = Mix_LoadWAV(alarm);
    if (beep == NULL) {
        printf("Unable to locate beep: %s", Mix_GetError());
        exit(1);
    }
}

void destroy_sound() {
    Mix_FreeChunk(beep);
    beep = NULL;
    Mix_Quit();
    unsetenv("DY_SOUND");
}

inline void play_sound() {
    Mix_PlayChannel(-1, beep, 0);
}