#include "graphics.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>

extern chip8_t chip8;

int create_window() {
    /* Initialise SDL Video and create Window, Renderer */
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Could not init SDL: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window *screen
        = SDL_CreateWindow("Kuro CHIP-8 Emulator", SDL_WINDOWPOS_UNDEFINED,
                           SDL_WINDOWPOS_UNDEFINED, WIN_W, WIN_H, 0);
    if (!screen) {
        fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
    }
    SDL_Renderer *renderer
        = SDL_CreateRenderer(screen, -1, SDL_RENDERER_SOFTWARE);

    /* Do renderer stuff I guess */
    SDL_SetRenderDrawColor(renderer, 46, 49, 49, 1);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    return 0;
}

void destroy_window(SDL_Window *screen, SDL_Renderer *renderer) {
    SDL_DestroyWindow(screen);
    SDL_Quit();
}

void draw_to_window(uint8_t x, uint8_t y, uint8_t n) {
    for (int row = 0; row < n; row++) {

        uint8_t sprite_in_memory = chip8.memory[chip8.index + row];

        for (int col = 0; col < 8; col++) {

            uint8_t sprite_pixel = sprite_in_memory & (0x80 >> col);
            bool *screen_pixel = &chip8.display[x + col][y + row];

            /* Check if sprite pixel is set */
            if (sprite_pixel) {
                /* Check if display pixel is set */
                if (*screen_pixel == true) {
                    chip8.registers[0xf] = 1;
                    *screen_pixel = false;
                }
                *screen_pixel = true;

                /* Draw here with SDL here */
            }
        }
    }
}