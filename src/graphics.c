#include "graphics.h"
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>

extern chip8_t chip8;

int create_window(SDL_Window *screen, SDL_Renderer *renderer,
                  SDL_Texture *texture) {
    /* Initialise SDL Video and create Window, Renderer */
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Could not init SDL: %s\n", SDL_GetError());
        return 1;
    }
    screen
        = SDL_CreateWindow("Kuro CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED,
                           SDL_WINDOWPOS_CENTERED, WIN_W * 20, WIN_H * 20, 0);
    if (!screen) {
        fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
    }

    renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_SOFTWARE);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                SDL_TEXTUREACCESS_TARGET, WIN_W, WIN_H);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderClear(renderer);


    return 0;
}

void destroy_window(SDL_Window *screen, SDL_Renderer *renderer,
                    SDL_Texture *texture) {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(screen);
    SDL_Quit();
}

void draw_to_window(SDL_Renderer *renderer, SDL_Texture *texture) {
    uint32_t *pixels = malloc(2048 * 4);

    for (int i = 0; i < 2048; i++) {
        pixels[i] = ((int32_t)chip8.display[i] << 23) >> 23;
    }

    SDL_UpdateTexture(texture, NULL, pixels, WIN_W * sizeof(uint32_t));
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}