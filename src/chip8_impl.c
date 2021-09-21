#include "chip8_impl.h"

void decode_and_execute() {

    /* Set higher byte and instruction nibble */
    uint8_t hb = (opcode_main >> 8) & 0xff;
    uint8_t inst_nib = (hb >> 4);
    uint8_t operand_X = (hb & 0x0f);

    /* lower byte */
    uint8_t lb = (opcode_main & 0xff);
    uint8_t operand_Y = (lb >> 4);
    uint8_t operand_N = (lb & 0x0f);
    uint16_t KK = lb;

    uint16_t NNN = operand_X << 8 | KK;

#ifdef DEBUG
    printf("0x%04x      %04x         ", chip8.PC - 2, opcode_main);
#endif


    /*Decoding here */
    switch (inst_nib) {
    case 0x00:
        switch (KK) {

        /* 00E0 */
        case 0xe0:
            memset(chip8.display, 0, (WIN_W * WIN_H));
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
        if ((chip8.registers[operand_X] ^ KK) == 0) {
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
        if ((chip8.registers[operand_X] ^ chip8.registers[operand_Y]) == 0) {
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
        switch (operand_N) {
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
        case 0x4: {

            uint8_t X, Y;
            X = chip8.registers[operand_X];
            Y = chip8.registers[operand_Y];

            /* ADD VX and VY and set VF */
            chip8.registers[operand_X] += chip8.registers[operand_Y];

            /* Check for overflow and set the flag */
            if (X > UINT8_MAX - Y) {
                chip8.registers[0xF] = 0x1;
            } else {
                chip8.registers[0xF] = 0x0;
            }

#ifdef DEBUG
            printf("ADD V%01x, V%01x\n", operand_X, operand_Y);
#endif
            break;
        }


        /* 8XY5 instruction */
        case 0x5: {
            /* Sub VY from VX and set VF */
            chip8.registers[0xF] = 1;
            uint8_t X, Y;
            X = chip8.registers[operand_X];
            Y = chip8.registers[operand_Y];

            chip8.registers[operand_X] -= chip8.registers[operand_Y];
            if (X < Y) {
                chip8.registers[0xF] = 0;
            }
#ifdef DEBUG
            printf("SUB V%01x, V%01x\n", operand_X, operand_Y);
#endif
            break;
        }

        /* 8XY6 instrcution */
        case 0x6:
            /* Check LSB is set or not, if set, set VF to 1 otherwise 0 */
            if (chip8.registers[operand_X] & 1) {
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
        case 0x7: {
            /* Sub VX from VY and set VF */
            chip8.registers[0xF] = 1;
            uint8_t X, Y;
            X = chip8.registers[operand_X];
            Y = chip8.registers[operand_Y];
            chip8.registers[operand_X]
                = chip8.registers[operand_Y] - chip8.registers[operand_X];

            if (X > Y) {
                chip8.registers[0xF] = 0;
            }

#ifdef DEBUG
            printf("SUBN V%01x, V%01x\n", operand_X, operand_Y);
#endif
            break;
        }

        /* 8XYE*/
        case 0xe:
            /* Check MSB is set or not, if set, set VF to 1 otherwise 0 */
            if (chip8.registers[operand_X] & (1 << 7)) {
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
        if ((chip8.registers[operand_X] ^ chip8.registers[operand_Y])) {
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


    /* BNNN */
    case 0x0b:
        /* Set program counter NNN + VX */
        chip8.PC = NNN + chip8.registers[0];
#ifdef DEBUG
        printf("JMP V0, 0x%03x\n", NNN);
#endif
        break;


    /* CXKK */
    case 0x0c:
        /* generate a random number between 0 and 255 and do Bitwise AND with KK
         */
        chip8.registers[operand_X] = (rand() % 256) & KK;
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

        for (int height = 0; height < operand_N; height++) {
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
        printf("DRW V%01x, V%01x, %01x\n", operand_X, operand_Y, operand_N);
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
             * and continue */
            chip8.PC -= 2;
            for (int i = 0; i < 16; i++) {
                if (chip8.key_states[i] == 1) {
                    chip8.registers[operand_X] = i;
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


        case 0x1E: {
            uint8_t X, I;
            X = chip8.registers[operand_X];
            I = chip8.index;

            /* Add I and Vx and store in I */
            chip8.index += chip8.registers[operand_X];

            /* check for overflow */
            if (I > UINT8_MAX - X) {
                chip8.registers[0xF] = 1;
            } else {
                chip8.registers[0xF] = 0;
            }
#ifdef DEBUG
            printf("ADD I, V%0x\n", operand_X);
#endif
            break;
        }


        case 0x29:
            /* Set index to sprite's address stored in VX*/
            chip8.index = (chip8.registers[operand_X] & 0xF) * 5;
#ifdef DEBUG
            printf("F%01x29 - LD F, V%0x\n", operand_X, operand_X);
#endif
            break;


        case 0x33: {
            /* Store decimal value present in VX in BSD order as Hundreds at I,
             * Tens at I+1, Ones at I+2 */

            /* Seperate out 3 digits from value stored at VX */
            uint8_t num_hundreds, num_tens, num_ones;

            uint8_t num = chip8.registers[operand_X];
            num_ones = num % 10;
            num /= 10;
            num_tens = num % 10;
            num_hundreds = num / 10;

            /* Store in BSD Order */
            chip8.memory[chip8.index] = num_hundreds;
            chip8.memory[chip8.index + 1] = num_tens;
            chip8.memory[chip8.index + 2] = num_ones;

#ifdef DEBUG
            printf("F%01x33 - LD B, V%01x\n", operand_X, operand_X);
            printf("[VX] - %03d\n", chip8.registers[operand_X]);
            printf("[I, I+1, I+2] - %d%d%d\n", chip8.memory[chip8.index],
                   chip8.memory[chip8.index + 1],
                   chip8.memory[chip8.index + 2]);
#endif
            break;
        }

        case 0x55:
            /*copies the values of registers V0 through Vx into memory, starting
             * at the address in I*/

            memcpy(&chip8.memory[chip8.index], chip8.registers, operand_X + 1);
#ifdef DEBUG
            printf("F%01x55 - LD [I], V%01x\n", operand_X, operand_X);
#endif
            break;


        case 0x65:
            /* copies the values starting at memory index I and stores them into
             * registers */

            memcpy(&chip8.registers[0], &chip8.memory[chip8.index],
                   operand_X + 1);

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
        printf("Invalid instruction: %04x\n", opcode_main);
        break;
    }
}
