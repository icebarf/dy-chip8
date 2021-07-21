#include "graphics.h"
#include "debug.h"
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>
#include <stdlib.h>

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

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR32,
                                SDL_TEXTUREACCESS_STREAMING, WIN_W, WIN_H);

    /* Set Texture to a solid color */
    int pitch;
    uint32_t *pixels = malloc(2048 * 4);

    for (int i = 0; i < WIN_W * WIN_H; i++) {
        pixels[i] = 0x808080ff;
    }

    SDL_UpdateTexture(texture, NULL, pixels, WIN_H * 4);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    free(pixels);
    return 0;
}

void destroy_window(SDL_Window *screen, SDL_Renderer *renderer,
                    SDL_Texture *texture) {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(screen);
    SDL_Quit();
}


void draw_to_window(SDL_Renderer *renderer, SDL_Texture *texture,
                    uint32_t *pixels) {

    for (int i = 0; i < 2048; i++) {
        if (chip8.display[i])
            pixels[i] = 0x000000FF;
        else
            pixels[i] = 0x808080FF;
    }

    SDL_UpdateTexture(texture, NULL, pixels, WIN_W * 4);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    /*print_screen();*/
}