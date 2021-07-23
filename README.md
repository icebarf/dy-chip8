# DY Chip-8

DY is a chip-8 interpreter (emulator) written in C. Graphics, Input, Sound have been implemented with the help SDL2. The emulator runs at 700Mhz using nanosleep per loop cycle. It has the following features as of in 4 days of development time.

## Features
- Runs at an oddball resolution of 1280*640
- Beep sound as with the original CHIP-8 interpreter
- Printing memory, instruction disassembly and final screen state to terminal for debugging (WIP)

## Showcase

#### [CHIP-8 Test Rom by corax89](https://github.com/corax89/chip8-test-rom)

![test_opcodes.png](img/test_opcodes.png)

#### [chip8-test-rom by metteo](https://github.com/metteo/chip8-test-rom)

![chip8-test-rom.png](img/chip8-test-rom.png)

#### [c8_test rom by Skosulor](https://github.com/Skosulor/c8int/tree/master/test)

![c8_test.png](img/c8_test.png)

#### [BestCoder's Test Rom](https://cdn.discordapp.com/attachments/465586212804100106/482263586547302426/BC_test.ch8)

![BC_test.png](img/BC_test.png)

> [Rom Description](https://cdn.discordapp.com/attachments/465586212804100106/482263582793531423/BC_test.txt)


## Usage

```bash
$ dy <path/to/rom/file> <path/to/sound/file>
```

## Dependencies
- SDL2
- SDL2 Mixer

## Installation

### Compile the emulator
- make emu

### Compile the emulator and install
- make emu_install

### Compile the emulator with optimizations
- make emu_opti

### Compile the emulator with optimizations and install
- make emu_opti_install

> Note: This copies the default sound file from src/sound to $HOME/.config/dy-chip8/sound/

### Compile the emulator with debugging to terminal and gdb symbols
- make emu_debug

### Compile the emulator with gdb symbols only
- make emu_gdb