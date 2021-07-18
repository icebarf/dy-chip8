CC = gcc
WARN = -Wall -Wextra
FLAGS = -O2

emu: src/*.c src/*.h
	$(CC) $(WARN) src/chip8.c src/decode.c src/execute.c src/fetch.c -o bin/emu

emu_debug: src/*.c src/*.h
	$(CC) $(WARN) src/chip8.c src/decode.c src/execute.c src/fetch.c -o bin/emu -g

clean:
	rm bin/*