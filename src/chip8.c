#include "chip8.h"
#include "decode.h"
#include "execute.h"
#include "fetch.h"
#include <stdint.h>
#include <sys/types.h>

chip8_t chip8 = {0};

/* enumerator for accessing single character row of font */
enum font_char {
    ZERO,
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    A,
    B,
    C,
    D,
    E,
    F
};

/* Fontset for chip8 */
uint8_t Font[16][5] = {
    {0xF0, 0x90, 0x90, 0x90, 0xF0}, // 0
    {0x20, 0x60, 0x20, 0x20, 0x70}, // 1
    {0xF0, 0x10, 0xF0, 0x80, 0xF0}, // 2
    {0xF0, 0x10, 0xF0, 0x10, 0xF0}, // 3
    {0x90, 0x90, 0xF0, 0x10, 0x10}, // 4
    {0xF0, 0x80, 0xF0, 0x10, 0xF0}, // 5
    {0xF0, 0x80, 0xF0, 0x90, 0xF0}, // 6
    {0xF0, 0x10, 0x20, 0x40, 0x40}, // 7
    {0xF0, 0x90, 0xF0, 0x90, 0xF0}, // 8
    {0xF0, 0x90, 0xF0, 0x10, 0xF0}, // 9
    {0xF0, 0x90, 0xF0, 0x90, 0x90}, // A
    {0xE0, 0x90, 0xE0, 0x90, 0xE0}, // B
    {0xF0, 0x80, 0x80, 0x80, 0xF0}, // C
    {0xE0, 0x90, 0x90, 0x90, 0xE0}, // D
    {0xF0, 0x80, 0xF0, 0x80, 0xF0}, // E
    {0xF0, 0x80, 0xF0, 0x80, 0x80}  // F
};

uint8_t keypad[4][4] = {{0x1, 0x2, 0x3, 0xC},  // 1 2 3 4
                        {0x4, 0x5, 0x6, 0xD},  // Q W E R
                        {0x7, 0x8, 0x9, 0xE},  // A S D F
                        {0xA, 0x0, 0xB, 0xF}}; // Z X C V


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
    fread(&chip8.memory[0x200], 1, file_size, fp);
    puts("Read ROM successfully");
    fclose(fp);
    return file_size;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: dy8 \"romfilename\"\n");
        return 0;
    }
    char *romname = argv[1];

    int read_size = fetchrom(romname);

    /* Set PC to point to 0x200 in memory */
    chip8.PC = &chip8.memory[0x200];

    /*for (;;) {
        fetch(read_size);
        decode();
        execute();
    }*/

    return 0;
}