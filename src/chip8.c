#include "chip8.h"
#include "chip8_impl.h"
#include "debug.h"
#include "graphics.h"
#include "keyboard.h"
#include "sound.h"

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif


/* Variables for emulator */
chip8_t chip8 = {0};
uint16_t opcode_main;
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

static inline int hz_to_ms(float hz) {
    int ms = (int)((1 / hz) * 1000);
    return ms;
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

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Usage: dy </path/to/rom> <optional: -hz integer_value> -s "
               "</path/to/sound>\n");
        return 0;
    }
    srand(time(NULL));

#if defined(_POSIX_VERSION) || defined(__linux__) || defined(TARGET_OS_MAC)
    /* Run at 700 Mhz */
    struct timespec tim;
    tim.tv_sec = 0;
    tim.tv_nsec = 1428571;

#endif

#if defined(_WIN32)
    /* Run at 700 Mhz */
    int ms = (int)((1.0f / 700.0f) * 1000);

#endif
        char *sound_path = NULL;

    /* Flag verification */
    if (argc > 2) {
        if (strcmp("-hz", argv[2]) == 0) {
            float hz = strtof(argv[3], NULL);

            if (hz < 0) {
                printf("Error: Invalid Value for hertz\n");
                return 0;
            }

#if defined(_POSIX_VERSION) || defined(__linux__) || defined(TARGET_OS_MAC)
            tim.tv_nsec = (1000000000 / hz);
#endif
#if defined(_WIN32)
            ms = hz_to_ms(hz);
#endif
        }
        if (strcmp("-s", argv[2]) == 0) {
            sound_path = argv[3];
        }


        if (argc > 4) {
            if (strcmp("-hz", argv[4]) == 0) {
                float hz = strtof(argv[5], NULL);

                if (hz < 0) {
                    printf("Error: Invalid Value for hertz\n");
                    return 0;
                }

#if defined(_POSIX_VERSION) || defined(__linux__) || defined(TARGET_OS_MAC)
                tim.tv_nsec = (1000000000 / hz);
#endif
#if defined(_WIN32)
                ms = hz_to_ms(hz);
#endif
            }

            if (strcmp("-s", argv[4]) == 0) {
                sound_path = argv[5];
            }
        }
    }


    /* SDL */
    SDL_Event event;

    create_window();
    init_sound(sound_path);

    /* Variables to be used in emulator */
    char *romname = argv[1];
    fetchrom(romname);

    /* Load fontset into memory at 0x0000 */
    memcpy(&chip8.memory, fontset, 80);
    int j = 0;
    for (int i = 0; i < 80; i += 5) {
        chip8.fontset[j++] = i;
    }

    /* Set program counter */
    chip8.PC = 0x0200;

    /* Timers */
    uint32_t prev_time = 0, current_time, difference;
    int32_t acc = 0;
    const float time_rate = 1000.0f / 60.0f;

#ifdef DEBUG
    log_to_terminal();
    print_memory();
    printf("Memory    Instruction    Disassembly\n");

#endif

    /* Emu Loop */

    bool run = true;
    while (run) {

        /* SDL Time */
        current_time = SDL_GetTicks(); // in milliseconds
        difference = current_time - prev_time;
        prev_time = current_time;
        acc += difference;

        /* SDL Input handling and graphics */
        SDL_PollEvent(&event);

        switch (event.type) {
        case SDL_QUIT:
            run = false;
            break;
        case SDL_KEYDOWN: {
            handle_input(1, &event);
            break;
        }
        case SDL_KEYUP: {
            handle_input(0, &event);
        }
        }

        if (chip8.draw) {
            draw_to_window(chip8.pixels);
            chip8.draw = false;
        }

        /* Main loop */
        while (1) {
            fetch();
            decode_and_execute();

            break;
        }

        /* Timers */
        while (acc >= time_rate) {
            if (chip8.delay_timer > 0) {
                --chip8.delay_timer;
            }

            if (chip8.sound_timer > 0) {
                play_sound();
                --chip8.sound_timer;
            }
            acc -= time_rate;
        }

#if defined(_POSIX_VERSION) || defined(__linux__) || defined(TARGET_OS_MAC)
        nanosleep(&tim, NULL);
#endif
#if defined(_WIN32)
        Sleep(ms);
#endif
    }

#ifdef DEBUG
    printf("\n\nFinal Screen Print\n");
    print_screen();
#endif

    destroy_sound();
    destroy_window();
    return 0;
}
