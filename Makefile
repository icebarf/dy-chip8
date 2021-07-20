CC = gcc
WARN = -Wall -Wextra
FLAGS = -O2
SDL = `pkg-config --cflags --libs sdl2`
emu: src/*.c src/*.h
	mkdir -p bin
	$(CC) $(WARN) src/chip8.c src/graphics.c -o bin/emu $(SDL)

emu_debug: src/*.c src/*.h
	mkdir -p bin
	$(CC) $(WARN) $(SDL) -DDEBUG src/chip8.c src/graphics.c src/debug.c -o bin/emu $(SDL) -g

clean:
	rm bin/*