#include "chip8.h"
#include "debug.h"
#include "graphics.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* NEEDS REFACTORING */


/* Variables for emulator */
chip8_t chip8 = {0};
uint16_t opcode_main, opcode;
int read_size;
int top = -1;

/* Fontset for chip8 */
uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

uint8_t keypad[16] = {
    0x01, 0x02, 0x03, 0x0C, // 1 2 3 4
    0x04, 0x05, 0x06, 0x0D, // Q W E R
    0x07, 0x08, 0x09, 0x0E, // A S D F
    0x0A, 0x00, 0x0B, 0x0F  // Z X C V

};

/* SDL Input Handler inline Routine */
static inline void handle_input(SDL_Event *event) {
    /* Row 1 */
    switch (event->key.keysym.sym) {
    case SDLK_1: {
        chip8.key_states[one_k] = 1;
        break;
    }
    case SDLK_2: {
        chip8.key_states[two_k] = 1;
        break;
    }
    case SDLK_3: {
        chip8.key_states[three_k] = 1;
        break;
    }
    case SDLK_4: {
        chip8.key_states[C_k] = 1;
        break;
    }
        /* Row 2 */
    case SDLK_q: {
        chip8.key_states[four_k] = 1;
        break;
    }
    case SDLK_w: {
        chip8.key_states[five_k] = 1;
        break;
    }
    case SDLK_e: {
        chip8.key_states[six_k] = 1;
        break;
    }
    case SDLK_r: {
        chip8.key_states[D_k] = 1;
        break;
    }
        /* Row 3 */
    case SDLK_a: {
        chip8.key_states[seven_k] = 1;
        break;
    }
    case SDLK_s: {
        chip8.key_states[eight_k] = 1;
        break;
    }
    case SDLK_d: {
        chip8.key_states[nine_k] = 1;
        break;
    }
    case SDLK_f: {
        chip8.key_states[E_k] = 1;
        break;
    }
    /* Row 4 */
    case SDLK_z: {
        chip8.key_states[A_k] = 1;
        break;
    }
    case SDLK_x: {
        chip8.key_states[zero_k] = 1;
        break;
    }
    case SDLK_c: {
        chip8.key_states[B_k] = 1;
        break;
    }
    case SDLK_v: {
        chip8.key_states[F_k] = 1;
        break;
    }
    }
}

/* Emulator routines */

int fetchrom(char *romname) {
    FILE *fp;
    fp = fopen(romname, "rb");
    if (fp == NULL) {
        printf("Error: failed to open \"%s\"\n", romname);
        return -1;
    }

    if (fseek(fp, 0L, SEEK_END) < 0) {
        printf("Error: Possibly corrupt rom\n");
        return -1;
    }
    int file_size = ftell(fp);
    if (fseek(fp, 0L, SEEK_SET) < 0) {
        printf("Error: Possibly corrupt rom\n");

        return -1;
    }

    /* Read rom into memory */
    fread(&chip8.memory[0x0200], 1, file_size, fp);
    fclose(fp);

    /* get instructions read */
    for (file_size = 0x0; chip8.memory[0x200 + file_size] != 0; file_size++)
        ;
    return file_size;
}

/* fetch routine */
static inline void fetch() {
    opcode_main = chip8.memory[chip8.PC] << 8 | chip8.memory[chip8.PC + 1];
    chip8.PC += 2;
}

void decode_and_execute() {

    /* Set higher byte and instruction nibble */
    opcode = opcode_main;
    uint8_t hb = (opcode >> 8) & 0xff;
    uint8_t inst_nib = (hb >> 4);
    uint8_t operand_X = (hb & 0x0f);

    /* lower byte */
    uint8_t lb = (opcode & 0xff);
    uint8_t operand_Y = (lb >> 4);
    uint8_t operand_right = (lb & 0x0f);
    uint16_t KK = lb;

    uint16_t NNN = operand_X << 8 | KK;

    /* Variables for FX33 instructions */
    uint8_t num, num_dig1, num_dig2, num_dig3;

#ifdef DEBUG
    printf("0x%04x    0x%04x         ", chip8.PC, opcode);
#endif


    /*Decoding here */
    switch (inst_nib) {
    case 0x00:
        switch (KK) {

        /* 00E0 */
        case 0xe0:
            memset(chip8.display, 0, WIN_W * WIN_H);
            chip8.draw = true;
#ifdef DEBUG
            printf("CLS\n");
#endif
            break;
        /* 00EE instruction */
        case 0xee:
            /* return from call */
            Pop(chip8.PC);
#ifdef DEBUG
            printf("RET\n");
#endif
            break;
#ifdef DEBUG
        default:
            puts("");
#endif
        }

        break;


    /* 1NNN instruction*/
    case 0x01:
        /* Set program counter to NNN */
        chip8.PC = NNN;
#ifdef DEBUG
        printf("JMP 0x%03x\n", NNN);
#endif
        break;


    /* 2NNN instruction */
    case 0x02:
        /*Push current PC to stack, set PC to NNN */
        Push(chip8.PC);
        chip8.PC = NNN;
#ifdef DEBUG
        printf("CALL 0x%03x\n", NNN);
#endif
        break;


    /* 3XKK instruction */
    case 0x03:
        /* If VX register is equal to KK then skip current instruction */
        if (!(chip8.registers[operand_X] ^ KK)) {
            chip8.PC += 2;
        }
#ifdef DEBUG
        printf("SE V%01x, 0x%02x\n", operand_X, KK);
#endif
        break;


    /* 4XKK instruction */
    case 0x04:
        /* If VX register is not equal to KK then skip current instruction
         */
        if (chip8.registers[operand_X] ^ KK) {
            chip8.PC += 2;
        }
#ifdef DEBUG
        printf("SNE V%01x, 0x%02x\n", operand_X, KK);
#endif
        break;


    /* 5XY0 instruction */
    case 0x05:
        /* If VX is equal to VY register then skip current instruction */
        if (!(chip8.registers[operand_X] ^ chip8.registers[operand_Y])) {
            chip8.PC += 2;
        }
#ifdef DEBUG
        printf("SE V%01x, 0x%01x\n", operand_X, operand_Y);
#endif
        break;


    /* 6XKK instruction */
    case 0x06:
        /* Put KK into X */
        chip8.registers[operand_X] = KK;
#ifdef DEBUG
        printf("LD V%01x, 0x%02x\n", operand_X, KK);
#endif
        break;


    /* 7XKK instruction */
    case 0x07:
        /* Add KK to VX then store result in VX */
        chip8.registers[operand_X] += KK;
#ifdef DEBUG
        printf("ADD V%01x, 0x%02x\n", operand_X, KK);
#endif
        break;


    /* 08 instructions */
    case 0x08:
        switch (operand_right) {
        /* 8XY0 instruction */
        case 0x0:
            /* Put value in VY to VX */
            chip8.registers[operand_X] = chip8.registers[operand_Y];
#ifdef DEBUG
            printf("LD V%01x, V%01x\n", operand_X, operand_Y);
#endif
            break;
        /* 8XY1 instruction */
        case 0x1:
            /* Bitwise OR on VX and VY then store result in VX */
            chip8.registers[operand_X]
                = chip8.registers[operand_X] | chip8.registers[operand_Y];
#ifdef DEBUG
            printf("OR V%01x, V%01x\n", operand_X, operand_Y);
#endif
            break;
        /* 8XY2 instruction */
        case 0x2:
            /* Bitwise AND on VX and VY then store result in VX */
            chip8.registers[operand_X]
                = chip8.registers[operand_X] & chip8.registers[operand_Y];
#ifdef DEBUG
            printf("AND V%01x, V%01x\n", operand_X, operand_Y);
#endif
            break;
        /* 8XY3 instruction */
        case 0x3:
            /* Bitwise XOR on VX and VY then store result in VX */
            chip8.registers[operand_X]
                = chip8.registers[operand_X] ^ chip8.registers[operand_Y];
#ifdef DEBUG
            printf("XOR V%01x, V%01x\n", operand_X, operand_Y);
#endif
            break;
        /* 8XY4 instruction */
        case 0x4:
            /* ADD VX and VY and set VF */
            chip8.registers[operand_X] += chip8.registers[operand_Y];
            /* Check for overflow and set the flag */
            if (chip8.registers[operand_X]
                > UINT8_MAX - chip8.registers[operand_Y]) {
                chip8.registers[0xF] = 0x1;
            } else {
                chip8.registers[0xF] = 0x0;
            }
#ifdef DEBUG
            printf("ADD V%01x, V%01x\n", operand_X, operand_Y);
#endif
            break;
        /* 8XY5 instruction */
        case 0x5:
            /* Sub VY from VX and set VF */
            chip8.registers[operand_X] -= chip8.registers[operand_Y];
            if (chip8.registers[operand_X] > chip8.registers[operand_Y]) {
                chip8.registers[0xF] = 0x1;
            } else {
                chip8.registers[0xF] = 0x1;
            }
#ifdef DEBUG
            printf("SUB V%01x, V%01x\n", operand_X, operand_Y);
#endif
            break;
        /* 8XY6 instrcution */
        case 0x6:
            /* Check LSB is set or not, if set, set VF to 1 otherwise 0 */
            if (chip8.registers[operand_X] & 0b00000001) {
                chip8.registers[0xF] = 0x1;
            } else {
                chip8.registers[0xF] = 0x0;
            }
            chip8.registers[operand_X] = chip8.registers[operand_X] >> 1;
#ifdef DEBUG
            printf("SHR V%01x, V%01x\n", operand_X, operand_Y);
#endif
            break;
        /* 8XY7 instruction */
        case 0x7:
            /* Sub VX from VY and set VF */
            chip8.registers[operand_X]
                = chip8.registers[operand_Y] - chip8.registers[operand_X];

            if (chip8.registers[operand_Y] > chip8.registers[operand_X]) {
                chip8.registers[0xF] = 0x1;
            } else {
                chip8.registers[0xF] = 0x0;
            }
#ifdef DEBUG
            printf("SUBN V%01x, V%01x\n", operand_X, operand_Y);
#endif
            break;

            /* 8XYE*/
        case 0xe:
            /* Check MSB is set or not, if set, set VF to 1 otherwise 0 */
            if (chip8.registers[operand_X] & 0b10000000) {
                chip8.registers[0xF] = 0x1;
            } else {
                chip8.registers[0xF] = 0x0;
            }
            chip8.registers[operand_X] = chip8.registers[operand_X] << 1;
#ifdef DEBUG
            printf("SHL V%01x, V%01x\n", operand_X, operand_Y);
#endif
            break;
#ifdef DEBUG
        default:
            puts("");
#endif
        }
        break;


    /* 9XY0 */
    case 0x09:
        /* if VX is not equal to VY then skip instruction */
        if (chip8.registers[operand_X] ^ chip8.registers[operand_Y]) {
            chip8.PC += 2;
        }
#ifdef DEBUG
        printf("SNE V%01x, V%01x\n", operand_X, operand_Y);
#endif
        break;


    /* ANNN*/
    case 0x0a:
        /* Set I (index register) to NNN */
        chip8.index = NNN;
#ifdef DEBUG
        printf("LD I, 0x%03x\n", NNN);
#endif
        break;


    /* BNNN*/
    case 0x0b:
        /* Set program counter NNN + V0 */
        chip8.PC = NNN + chip8.registers[0];
#ifdef DEBUG
        printf("JMP V0, 0x%03x\n", NNN);
#endif
        break;


    /* CXKK */
    case 0x0c:
        /* generate a random number between 0 and 255 and do Bitwise AND with KK
         */
        chip8.registers[operand_X] = (rand() % (256) + 1) & KK;
#ifdef DEBUG
        printf("RND V%01x, 0x%02x\n", operand_X, KK);
#endif
        break;


    /* DXYN */
    case 0x0d: {
        uint16_t pixel;
        chip8.registers[0xF] = 0;
        uint8_t X = chip8.registers[operand_X] & 63;
        uint8_t Y = chip8.registers[operand_Y] & 31;

        for (int height = 0; height < operand_right; height++) {
            pixel = chip8.memory[chip8.index + height];

            for (int width = 0; width < 8; width++) {
                if ((pixel & (0x80 >> width)) != 0) {
                    if (chip8.display[(X + width + (Y + height) * WIN_W)]
                        == 1) {
                        chip8.registers[0xF] = 1;
                    }

                    chip8.display[(X + width + (Y + height) * WIN_W)] ^= 1;
                }
            }
        }
        chip8.draw = true;
    }
#ifdef DEBUG
        printf("DRW V%01x, V%01x, %01x\n", operand_X, operand_Y, operand_right);
        /*print_screen();*/
#endif
        break;


    /* 0E instructions */
    case 0x0e:

        switch (KK) {

        /* EX9E */
        case 0x9e: {
            /* Increment PC if key is in down state - Down means pressed or 1*/
            if (chip8.key_states[chip8.registers[operand_X]] == 1) {
                chip8.PC += 2;
            }
            break;
        }

#ifdef DEBUG
            printf("EX9E - SKP V%01x\n", operand_X);
#endif
            break;

        /* EXA1 */
        case 0xA1:
            /* Increment PC if key is in up state - up means not pressed or 0*/
            if (chip8.key_states[chip8.registers[operand_X]] == 0) {
                chip8.PC += 2;
            }

#ifdef DEBUG
            printf("SKNP V%01x\n", operand_X);
#endif
            break;
#ifdef DEBUG
        default:
            puts("");
#endif
        }
        break;


    /* 0F instructions */
    case 0x0f:

        switch (KK) {
        case 0x07:
            /* Set VX to Delay Timer (DT) */
            chip8.registers[operand_X] = chip8.delay_timer;
#ifdef DEBUG
            printf("LD V%01x, DT\n", operand_X);
#endif
            break;


        case 0x0a:
            /* Until any key is pressed, pause exec, after press set VX to key
             */
            chip8.PC -= 2;
            for (int i = 0; i < 16; i++) {
                if (chip8.key_states[i] == 1) {
                    chip8.registers[operand_X] = chip8.keyboard[i];
                    chip8.PC += 2;
                }
            }
#ifdef DEBUG
            printf("LD V%01x, K\n", operand_X);
#endif
            break;


        case 0x15:
            /* Set delay timer to VX */
            chip8.delay_timer = chip8.registers[operand_X];
#ifdef DEBUG
            printf("LD DT, V%0x\n", operand_X);
#endif
            break;
        case 0x18:
            /* Set Sound timer (ST) to Vx */
            chip8.sound_timer = chip8.registers[operand_X];
#ifdef DEBUG
            printf("LD ST, V%0x\n", operand_X);
#endif
            break;


        case 0x1E:
            /* Add I and Vx and store in I */
            chip8.index += chip8.registers[operand_X];

            /* Check for overflow and set VF */
            if (chip8.index > 0x0FFF - chip8.registers[operand_X]) {
                chip8.registers[0xF] = 0x1;
            } else {
                chip8.registers[0xF] = 0x0;
            }
#ifdef DEBUG
            printf("ADD I, V%0x\n", operand_X);
#endif
            break;


        case 0x29:
            /* Set index to sprite's address stored in VX*/
            chip8.index = chip8.memory[chip8.fontset[operand_X]];
#ifdef DEBUG
            printf("F%01x18 - LD F, V%0x\n", operand_X, operand_X);
#endif
            break;


        case 0x33:
            /* Store decimal value present in VX in BSD order as Hundreds at I,
             * Tens at I+1, Ones at I+2 */

            /* Seperate out 3 digits from value stored at VX */
            num = chip8.memory[operand_X];
            num_dig1 = num % 10;
            num /= 10;
            num_dig2 = num % 10;
            num_dig3 = num / 10;

            /* Store in BSD Order */
            chip8.memory[chip8.index] = num_dig1;
            chip8.memory[chip8.index + 1] = num_dig2;
            chip8.memory[chip8.index + 2] = num_dig3;
#ifdef DEBUG
            printf("F%01x33 - LD B, V%01x\n", operand_X, operand_X);
#endif
            break;


        case 0x55:
            /*copies the values of registers V0 through Vx into memory, starting
             * at the address in I*/
            memcpy(&chip8.memory[chip8.index], chip8.registers, 16);
#ifdef DEBUG
            printf("F%01x55 - LD [I], V%01x\n", operand_X, operand_X);
#endif
            break;


        case 0x65:
            /* copies the values starting at memory index I and stores them into
             * registers */
            memcpy(&chip8.registers[0], &chip8.memory[chip8.index], 16);
#ifdef DEBUG
            printf("F%01x65 - LD V%01x, [I]\n", operand_X, operand_X);
#endif
            break;
#ifdef DEBUG
        default:
            puts("");
#endif
        }
        break;


    default:
        printf("Invalid instruction: %04x\n", opcode);
        break;
    }
}


int main(int argc, char **argv) {

    if (argc < 2) {
        printf("Usage: emu \"romfilename\"\n");
        return 0;
    }
    srand(time(NULL));

    /* SDL */
    SDL_Window *screen = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;
    SDL_Event event;

    create_window(screen, renderer, texture);

    /* Variables to be used in emulator */
    char *romname = argv[1];
    read_size = fetchrom(romname);

    /* Load fontset into memory at 0x0000 */
    memcpy(&chip8.memory, fontset, 80);
    uint8_t font_mem_loc[16] = {0x0,  0x6,  0xb,  0x10, 0x15, 0x1a, 0x1f, 0x24,
                                0x29, 0x2e, 0x33, 0x38, 0x3D, 0x42, 0x47, 0x4C};
    memcpy(&chip8.fontset, font_mem_loc, 16);

    /* Set program counter */
    chip8.PC = 0x0200;

#ifdef DEBUG
    log_to_terminal();
    print_memory();
    printf("Memory    Instruction    Disassembly\n");

#endif

    /* Emu Loop */

    bool run = true;
    while (run) {

        SDL_PollEvent(&event);
        switch (event.type) {

        case SDL_QUIT:
            run = false;
            break;
        case SDL_KEYDOWN: {
            handle_input(&event);
            break;
        }
        }

        if (chip8.draw) {
            draw_to_window(renderer, texture, chip8.pixels);
            chip8.draw = false;
        }
        fetch();
        decode_and_execute();

        if (chip8.delay_timer > 0) {
            --chip8.delay_timer;
        }
        if (chip8.sound_timer > 0) {
            --chip8.sound_timer;
        }

        usleep(1);
    }
    printf("\n\nFinal Screen Print\n");
    print_screen();
    destroy_window(screen, renderer, texture);
    return 0;
}