#include "keyboard.h"
#include <SDL2/SDL_events.h>

extern chip8_t chip8;

/* SDL Input Handler inline Routine */
static inline void key_down(SDL_Event *event) {
    /* Row 1 */
    switch (event->key.keysym.scancode) {
    case SDL_SCANCODE_1: {
        chip8.key_states[one_k] = 1;
        break;
    }
    case SDL_SCANCODE_2: {
        chip8.key_states[two_k] = 1;
        break;
    }
    case SDL_SCANCODE_3: {
        chip8.key_states[three_k] = 1;
        break;
    }
    case SDL_SCANCODE_4: {
        chip8.key_states[C_k] = 1;
        break;
    }
        /* Row 2 */
    case SDL_SCANCODE_Q: {
        chip8.key_states[four_k] = 1;
        break;
    }
    case SDL_SCANCODE_W: {
        chip8.key_states[five_k] = 1;
        break;
    }
    case SDL_SCANCODE_E: {
        chip8.key_states[six_k] = 1;
        break;
    }
    case SDL_SCANCODE_R: {
        chip8.key_states[D_k] = 1;
        break;
    }
        /* Row 3 */
    case SDL_SCANCODE_A: {
        chip8.key_states[seven_k] = 1;
        break;
    }
    case SDL_SCANCODE_S: {
        chip8.key_states[eight_k] = 1;
        break;
    }
    case SDL_SCANCODE_D: {
        chip8.key_states[nine_k] = 1;
        break;
    }
    case SDL_SCANCODE_F: {
        chip8.key_states[E_k] = 1;
        break;
    }
    /* Row 4 */
    case SDL_SCANCODE_Z: {
        chip8.key_states[A_k] = 1;
        break;
    }
    case SDL_SCANCODE_X: {
        chip8.key_states[zero_k] = 1;
        break;
    }
    case SDL_SCANCODE_C: {
        chip8.key_states[B_k] = 1;
        break;
    }
    case SDL_SCANCODE_V: {
        chip8.key_states[F_k] = 1;
        break;
    }
    default:
        break;
    }
}

static inline void key_up(SDL_Event *event) {
    switch (event->key.keysym.scancode) {

    case SDL_SCANCODE_1: {
        chip8.key_states[one_k] = 0;
        break;
    }
    case SDL_SCANCODE_2: {
        chip8.key_states[two_k] = 0;
        break;
    }
    case SDL_SCANCODE_3: {
        chip8.key_states[three_k] = 0;
        break;
    }
    case SDL_SCANCODE_4: {
        chip8.key_states[C_k] = 0;
        break;
    }
        /* Row 2 */
    case SDL_SCANCODE_Q: {
        chip8.key_states[four_k] = 0;
        break;
    }
    case SDL_SCANCODE_W: {
        chip8.key_states[five_k] = 0;
        break;
    }
    case SDL_SCANCODE_E: {
        chip8.key_states[six_k] = 0;
        break;
    }
    case SDL_SCANCODE_R: {
        chip8.key_states[D_k] = 0;
        break;
    }
        /* Row 3 */
    case SDL_SCANCODE_A: {
        chip8.key_states[seven_k] = 0;
        break;
    }
    case SDL_SCANCODE_S: {
        chip8.key_states[eight_k] = 0;
        break;
    }
    case SDL_SCANCODE_D: {
        chip8.key_states[nine_k] = 0;
        break;
    }
    case SDL_SCANCODE_F: {
        chip8.key_states[E_k] = 0;
        break;
    }
    /* Row 4 */
    case SDL_SCANCODE_Z: {
        chip8.key_states[A_k] = 0;
        break;
    }
    case SDL_SCANCODE_X: {
        chip8.key_states[zero_k] = 0;
        break;
    }
    case SDL_SCANCODE_C: {
        chip8.key_states[B_k] = 0;
        break;
    }
    case SDL_SCANCODE_V: {
        chip8.key_states[F_k] = 0;
        break;
    }

    default:
        break;
    }
}

void handle_input(int state, SDL_Event *event) {
    (state == 0) ? key_up(event) : key_down(event);
}