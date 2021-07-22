CC = gcc
WARN = -Wall -Wextra
FLAGS = -march=native -O2 -lSDL2_mixer
SDL = `pkg-config --cflags --libs sdl2`

emu: src/*.c src/*.h
	mkdir -p bin
	$(CC) $(WARN) -lSDL2_mixer src/chip8.c src/graphics.c src/sound.c src/keyboard.c src/debug.c -o bin/dy $(SDL)


emu_opti: src/*.c src/*.h
	mkdir -p bin
	$(CC) $(WARN) $(FLAGS) src/chip8.c src/graphics.c src/sound.c src/keyboard.c src/debug.c -o bin/dy $(SDL)

emu_debug: src/*.c src/*.h
	mkdir -p bin
	$(CC) $(WARN) -lSDL2_mixer -DDEBUG src/chip8.c src/graphics.c src/sound.c src/keyboard.c src/debug.c -o bin/dy $(SDL) -g

emu_gdb: src/*.c src/*.h
	mkdir -p bin
	$(CC) $(WARN) -lSDL2_mixer src/chip8.c src/graphics.c src/sound.c src/keyboard.c src/debug.c -o bin/dy $(SDL) -g

emu_install: src/*.c src/*.h
	mkdir -p bin
	$(CC) $(WARN) $(FLAGS) src/chip8.c src/graphics.c src/sound.c src/keyboard.c src/debug.c -o bin/dy $(SDL)	
	chmod 755 bin/dy
	mkdir -p $$HOME/.config
	mkdir -p $$HOME/.config/dy-chip8
	mkdir -p $$HOME/.config/dy-chip8/sound
	cp src/sound/edited_alarm.wav $$HOME/.config/dy-chip8/sound/edited_alarm.wav
	sudo mv bin/dy /usr/bin/

clean:
	rm bin/*

clean_install:
	sudo rm -r $$HOME/.config/dy-chip8
	sudo rm /usr/bin/dy