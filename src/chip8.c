#include "chip8.h"
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
    0x00F0, 0x0090, 0x0090, 0x0090, 0x00F0, // 0
    0x0020, 0x0060, 0x0020, 0x0020, 0x0070, // 1
    0x00F0, 0x0010, 0x00F0, 0x0080, 0x00F0, // 2
    0x00F0, 0x0010, 0x00F0, 0x0010, 0x00F0, // 3
    0x0090, 0x0090, 0x00F0, 0x0010, 0x0010, // 4
    0x00F0, 0x0080, 0x00F0, 0x0010, 0x00F0, // 5
    0x00F0, 0x0080, 0x00F0, 0x0090, 0x00F0, // 6
    0x00F0, 0x0010, 0x0020, 0x0040, 0x0040, // 7
    0x00F0, 0x0090, 0x00F0, 0x0090, 0x00F0, // 8
    0x00F0, 0x0090, 0x00F0, 0x0010, 0x00F0, // 9
    0x00F0, 0x0090, 0x00F0, 0x0090, 0x0090, // A
    0x00E0, 0x0090, 0x00E0, 0x0090, 0x00E0, // B
    0x00F0, 0x0080, 0x0080, 0x0080, 0x00F0, // C
    0x00E0, 0x0090, 0x0090, 0x0090, 0x00E0, // D
    0x00F0, 0x0080, 0x00F0, 0x0080, 0x00F0, // E
    0x00F0, 0x0080, 0x00F0, 0x0080, 0x0080  // F
};

uint8_t keypad[4][4] = {
    {0x0001, 0x0002, 0x0003, 0x000C}, // 1 2 3 4
    {0x0004, 0x0005, 0x0006, 0x000D}, // Q W E R
    {0x0007, 0x0008, 0x0009, 0x000E}, // A S D F
    {0x000A, 0x0000, 0x000B, 0x000F}  // Z X C V

};
/* Emulator routines */

int fetchrom(char *romname) {
    /* Open fiile into fp */
    FILE *fp;
    fp = fopen(romname, "rb");
    if (fp == NULL) {
        printf("Error: failed to open \"%s\"\n", romname);
        return -1;
    }

    /* Get file size */
    if (fseek(fp, 0L, SEEK_END) < 0) {
        printf("Error: Possibly corrupt rom\n");
        return -1;
    }
    /* Store file size */
    int file_size = ftell(fp);
    assert(file_size > 0);
    /* Go back again*/
    if (fseek(fp, 0L, SEEK_SET) < 0) {
        printf("Error: Possibly corrupt rom\n");

        return -1;
    }

    /* Read rom into memory */
    fread(&chip8.memory[0x0200], 1, file_size, fp);
    puts("Read ROM successfully");
    fclose(fp);
    return file_size;
}

/* fetch routine */
void fetch() {
    opcode = chip8.memory[chip8.PC] << 8 | chip8.memory[chip8.PC + 1];
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

    /*Debugging*/
    /*printf("%04x\t\t\t%02x\t\t %02x\t\t\t\t", chip8.PC, inst[1], inst[0]);
    printf("%01x", inst_nib);
    printf("\t\t\t\t\t%01x\t", operand_X);
    printf("%01x\t", operand_Y);
    printf("%01x\t\t", operand_right);*/
    /*printf("%04x\n", NNN);*/

    /*Decoding here */
    switch (inst_nib) {
    case 0x00:
        switch (KK) {
        /* 00E0 instruction */
        case 0xe0:
            /*puts("00E0 - CLS");*/
            break;
        /* 00EE instruction */
        case 0xee:
            Pop(chip8.PC);
            /*puts("00EE - RET");*/
            break;
            // default:
            /*puts("");*/
        }

        break;

    /* 1NNN instruction*/
    case 0x01:
        /* Set program counter to NNN */
        chip8.PC = NNN;
        /*printf("1%03x - JMP %03x\n", NNN, NNN);*/
        break;
    /* 2NNN instruction */
    case 0x02:
        /*Push current PC to stack, set PC to NNN */
        Push(chip8.PC);
        chip8.PC = NNN;
        /*printf("2%03x - CALL %03x\n", NNN, NNN);*/
        break;

    /* 3XKK instruction */
    case 0x03:
        /* If VX register is equal to KK then skip current instruction */
        if (!(chip8.registers[operand_X] ^ KK)) {
            chip8.PC += 2;
        }
        /*printf("3%03x - SE V%01x, %02x\n", NNN, operand_X, KK);*/
        break;

    /* 4XKK instruction */
    case 0x04:
        /* If VX register is not equal to KK then skip current instruction
         */
        if (chip8.registers[operand_X] ^ KK) {
            chip8.PC += 2;
        }
        /*printf("3%03x - SNE V%01x, %02x\n", NNN, operand_X, KK);*/
        break;

    /* 5XY0 instruction */
    case 0x05:
        /* If VX is equal to VY register then skip current instruction */
        if (!(chip8.registers[operand_X] ^ chip8.registers[operand_Y])) {
            chip8.PC += 2;
        }
        /*printf("5%01x%01x0 - SE V%01x, %01x\n", operand_X, operand_Y,
           operand_X, operand_Y);*/
        break;

    /* 6XKK instruction */
    case 0x06:
        /* Put KK into X */
        chip8.registers[operand_X] = KK;
        // printf("6%01x%02x - LD V%01x, %02x\n", operand_X, KK, operand_X, KK);
        break;

    /* 7XKK instruction */
    case 0x07:
        /* Add KK to VX then store result in VX */
        chip8.registers[operand_X] += KK;
        // printf("7%01x%02x - ADD V%01x, %02x\n", operand_X, KK, operand_X,
        // KK);
        break;

    /* 08 instructions */
    case 0x08:
        switch (operand_right) {
        /* 8XY0 instruction */
        case 0x0:
            /* Put value in VY to VX */
            chip8.registers[operand_X] = chip8.registers[operand_Y];
            //  printf("8%01x%01x0 - LD V%01x, V%01x\n", operand_X, operand_Y,
            //       operand_X, operand_Y);
            break;
        /* 8XY1 instruction */
        case 0x1:
            /* Bitwise OR on VX and VY then store result in VX */
            chip8.registers[operand_X]
                = chip8.registers[operand_X] | chip8.registers[operand_Y];
            // printf("8%01x%01x1 - OR V%01x, V%01x\n", operand_X, operand_Y,
            //     operand_X, operand_Y);
            break;
        /* 8XY2 instruction */
        case 0x2:
            /* Bitwise AND on VX and VY then store result in VX */
            chip8.registers[operand_X]
                = chip8.registers[operand_X] & chip8.registers[operand_Y];
            // printf("8%01x%01x2 - AND V%01x, V%01x\n", operand_X, operand_Y,
            //     operand_X, operand_Y);
            break;
        /* 8XY3 instruction */
        case 0x3:
            /* Bitwise XOR on VX and VY then store result in VX */
            chip8.registers[operand_X]
                = chip8.registers[operand_X] ^ chip8.registers[operand_Y];
            // printf("8%01x%01x3 - XOR V%01x, V%01x\n", operand_X, operand_Y,
            //      operand_X, operand_Y);
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
            // printf("8%01x%01x4 - ADD V%01x, V%01x\n", operand_X, operand_Y,
            //     operand_X, operand_Y);
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
            // printf("8%01x%01x5 - SUB V%01x, V%01x\n", operand_X, operand_Y,
            //     operand_X, operand_Y);
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
            // printf("8%01x%01x6 - SHR V%01x, V%01x\n", operand_X, operand_Y,
            //     operand_X, operand_Y);
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
            // printf("8%01x%01x7 - SUBN V%01x, V%01x\n", operand_X, operand_Y,
            //     operand_X, operand_Y);
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
            // printf("8%01x%01xe - SHL V%01x, V%01x\n", operand_X, operand_Y,
            //     operand_X, operand_Y);
            break;
        default:
            puts("");
        }
        break;
    /* 9XY0 */
    case 0x09:
        /* if VX is not equal to VY then skip instruction */
        if (chip8.registers[operand_X] ^ chip8.registers[operand_Y]) {
            chip8.PC += 2;
        }
        // printf("9%01x%01x0 - SNE V%01x, V%01x\n", operand_X, operand_Y,
        //     operand_X, operand_Y);
        break;
    /* ANNN*/
    case 0x0a:
        /* Set I (index register) to NNN */
        chip8.index = NNN;
        // printf("A%03x - LD I, %03x\n", NNN, NNN);
        break;

    /* BNNN*/
    case 0x0b:
        /* Set program counter NNN + V0 */
        chip8.PC = NNN + chip8.registers[0];
        // printf("B%03x - JP V0, %03x\n", NNN, NNN);
        break;
    /* CXKK */
    case 0x0c:
        /* generate a random number between 0 and 255 and do Bitwise AND with KK
         */
        chip8.registers[operand_X] = (rand() % (256) + 1) & KK;
        // printf("C%01x%02x - RND V%01x, %02x\n", operand_X, KK, operand_X,
        // KK);
        break;
    /* DXYN */
    case 0x0d:
        /* Set index to point at where font is */
        chip8.index = 0x0000;
        draw_to_window((chip8.registers[operand_X] & 64),
                       (chip8.registers[operand_Y] & 32), operand_right);
        // printf("D%03x - DRW V%01x, V%01x, %01x\n", NNN, operand_X, operand_Y,
        //     operand_right);
        break;
    /* 0E instructions */
    case 0x0e:
        switch (KK) {
        case 0x9e:
            // printf("EX9E - SKP Vx - Not implemented \n");
            break;
        case 0xA1:
            // printf("EXA1 - SKNP Vx - Not implemented \n");
            break;
            // default:
            // puts("");
        }
        break;
    /* 0F instructions */
    case 0x0f:
        switch (KK) {
        case 0x07:
            /* Set VX to Delay Timer (DT) */
            chip8.registers[operand_X] = chip8.delay_timer;
            //    printf("F%01x07 - LD V%01x, DT\n", operand_X, operand_X);
            break;
        case 0x0a:
            //  printf("FX0A - LD Vx, K - Not implemented \n");
            break;
        case 0x15:
            chip8.delay_timer = chip8.registers[operand_X];
            // printf("F%01x15 - LD DT, V%0x\n", operand_X, operand_X);
            break;
        case 0x18:
            /* Set Sound timer (ST) to Vx */
            chip8.sound_timer = chip8.registers[operand_X];
            // printf("F%01x18 - LD ST, V%0x\n", operand_X, operand_X);
            break;
        case 0x1E:
            /* Add I and Vx and store in I */
            chip8.index += chip8.registers[operand_X];
            // printf("F%01x1E - ADD I, V%0x\n", operand_X, operand_X);
            break;
        case 0x29:
            /* Set index to sprite's address stored in VX*/
            chip8.index = chip8.registers[operand_X];
            // printf("F%01x18 - LD F, V%0x\n", operand_X, operand_X);
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
            // printf("F%01x33 - LD B, V%01x\n", operand_X, operand_X);
            break;

        case 0x55:
            /*copies the values of registers V0 through Vx into memory, starting
             * at the address in I*/
            for (int i = 0; i < 16; i += 4) {
                chip8.memory[chip8.index] = chip8.registers[i];
                chip8.memory[chip8.index + 1] = chip8.registers[i + 1];
                chip8.memory[chip8.index + 2] = chip8.registers[i + 2];
                chip8.memory[chip8.index + 3] = chip8.registers[i + 3];
            }
            // printf("F%01x55 - LD [I], V%01x\n", operand_X, operand_X);
            break;
        case 0x65:
            /* copies the values starting at memory index I and stores them into
             * registers */
            for (int i = 0; i < 20; i += 4) {
                chip8.registers[i] = chip8.memory[chip8.index];
                chip8.registers[i + 1] = chip8.memory[chip8.index + 1];
                chip8.registers[i + 2] = chip8.memory[chip8.index + 2];
                chip8.registers[i + 3] = chip8.memory[chip8.index + 3];
            }
            // printf("F%01x65 - LD V%01x, [I]\n", operand_X, operand_X);
            break;
            // default:
            //  puts("");
        }
        break;
    default:
        printf("Invalid instruction: %04x\n", opcode);
        break;
    }


    chip8.PC += 2;
}

void execute() {
    /* handle execution of decoded instructions */
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
    memcpy(&chip8.memory[0x0], fontset, 80);

    /* Set program counter */
    chip8.PC = 0x0200;

    /* Emu Loop */
    int i = 0;
    /*printf("NNN\n");*/
    /*printf("Program Counter\tHighByte LowByte\t\tInstruction "
           "nibble\tX\tY\tLast\tAssembly\n");*/
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
    print_screen();
    return 0;
}