#include "sound.h"

Mix_Chunk *beep = NULL;

void init_sound(char *sound_path) {
    /* Initialise Mixer */
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("Unable to initialise sound: %s", SDL_GetError());
        exit(1);
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Unable to initialise SDL Mixer: %s", Mix_GetError());
        exit(1);
    }


    if(sound_path == NULL){
        sound_path = "/usr/share/dy-chip8/edited_alarm.wav";
    }
  
    beep = Mix_LoadWAV(sound_path);
  
    if (beep == NULL) {
        printf("Unable to locate beep: %s", Mix_GetError());
        exit(1);
    }
}

void destroy_sound() {
    Mix_FreeChunk(beep);
    beep = NULL;
    Mix_Quit();
}

inline void play_sound() {
    Mix_PlayChannel(-1, beep, 0);
}
