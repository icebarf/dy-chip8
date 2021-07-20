#include "chip8.h"
#include "debug.h"
#include "graphics.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Variables for emulator */
chip8_t chip8 = {0};
uint16_t opcode;
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

uint8_t keypad[4][4] = {
    {0x01, 0x02, 0x03, 0x0C}, // 1 2 3 4
    {0x04, 0x05, 0x06, 0x0D}, // Q W E R
    {0x07, 0x08, 0x09, 0x0E}, // A S D F
    {0x0A, 0x00, 0x0B, 0x0F}  // Z X C V

};
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
    return file_size;
}

/* fetch routine */
static inline void fetch() {
    opcode = chip8.memory[chip8.PC] << 8 | chip8.memory[chip8.PC + 1];
    chip8.PC += 2;
}

void decode_and_execute() {

    /* Store the first nibble of instruction */
    uint8_t *inst = &opcode;

    /* Get instruction */
    uint8_t inst_nib = (inst[1] >> 4);

    /* higher byte */
    uint16_t KK = inst[0];
    uint8_t operand_X = (inst[1] & 0x0f);

    /* lower byte */
    uint8_t operand_Y = (inst[0] >> 4);
    uint8_t operand_right = (inst[0] & 0x0f);

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
#ifdef DEBUG
            printf("CLS\n");
#endif
            break;
        /* 00EE instruction */
        case 0xee:
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
    case 0x0d:
        /* Set index to point at where font is */
        chip8.index = 0x0000;
        draw_to_window((chip8.registers[operand_X] & 64),
                       (chip8.registers[operand_Y] & 32), operand_right);
#ifdef DEBUG
        printf("DRW V%01x, V%01x, %01x\n", operand_X, operand_Y, operand_right);
#endif
        break;
    /* 0E instructions */
    case 0x0e:
        switch (KK) {
        case 0x9e:
#ifdef DEBUG
            printf("EX9E - SKP V%01x\n", operand_X);
#endif
            break;
        case 0xA1:
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
#ifdef DEBUG
            printf("LD V%01x, K\n", operand_X);
#endif
            break;
        case 0x15:
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
            chip8.index = chip8.registers[operand_X];
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
    /* Variables to be used in emulator */
    char *romname = argv[1];
    read_size = fetchrom(romname);

    /* Load fontset into memory at 0x0000 */
    memcpy(&chip8.memory, fontset, 80);

    /* Set program counter */
    chip8.PC = 0x0200;

#ifdef DEBUG
    log_to_terminal();
    print_memory();
    printf("Memory    Instruction    Disassembly\n");
#endif

/* Emu Loop */
#ifdef DEBUG
    int i = 0;
    int read_upto = 0x200 + read_size;

    while (i < read_upto) {
        srand(time(NULL));

        /* emulator functions */
        fetch();
        decode_and_execute();

        /* timers and PC*/
        if (chip8.delay_timer > 0) {
            --chip8.delay_timer;
        }
        if (chip8.sound_timer > 0) {
            --chip8.sound_timer;
        }

        /* Run at 1 Mhz or 10^-6 times a second */
        usleep(1);
        i++;
    }
#endif

#ifndef DEBUG
    while (1) {
        srand(time(NULL));

        /* emulator functions */
        fetch();
        decode_and_execute();

        /* timers and PC*/
        if (chip8.delay_timer > 0) {
            --chip8.delay_timer;
        }
        if (chip8.sound_timer > 0) {
            --chip8.sound_timer;
        }

        /* Run at 1 Mhz or 10^-6 times a second */
        usleep(1);
    }
#endif
    return 0;
}