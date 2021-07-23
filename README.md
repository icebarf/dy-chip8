# DY Chip-8

DY is a chip-8 interpreter (emulator) written in C. Graphics, Input, Sound have been implemented with the help SDL2. The emulator runs at 700Mhz using nanosleep per loop cycle. It has the following features as of in 4 days of development time.

## Features
- Runs at an oddball resolution of 1280*640
- Beep sound as with the original CHIP-8 interpreter
- Printing memory, disassembly and final screen state terminal for debugging (WIP)

## Usage

```bash
$ dy <path/to/rom/file> <path/to/sound/file>
```
> Note: No environmental variables are supported as SDL2 looks for the file in the current directory or the absolute path if starts with `/`

## Dependencies
- SDL2

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

> Note: This will not compile for windows. If you can make nanosleep() available for your platform, then you can compile this.