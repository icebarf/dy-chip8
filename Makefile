CC = gcc
WARN = -Wall -Wextra
FLAGS = -march=native -O2 -l SDL2_mixer
SDL = `pkg-config --cflags --libs sdl2`

emu: src/*.c src/*.h
	mkdir -p bin
	$(CC) $(WARN) -l SDL2_mixer src/chip8.c src/chip8_impl.c src/graphics.c src/sound.c src/keyboard.c src/debug.c -o bin/dy $(SDL)

emu_win: src/*.c src/*.h
	mkdir -p bin
	x86_64-w64-mingw32-gcc $(WARN) src/*.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -o bin/dy.exe

emu_opti: src/*.c src/*.h
	mkdir -p bin
	$(CC) $(WARN) $(FLAGS) src/chip8.c src/chip8_impl.c src/graphics.c src/sound.c src/keyboard.c src/debug.c -o bin/dy $(SDL)

emu_debug: src/*.c src/*.h
	mkdir -p bin
	$(CC) $(WARN) -lSDL2_mixer -DDEBUG src/chip8.c src/chip8_impl.c src/graphics.c src/sound.c src/keyboard.c src/debug.c -o bin/dy $(SDL) -g

emu_gdb: src/*.c src/*.h
	mkdir -p bin
	$(CC) $(WARN) -lSDL2_mixer src/chip8.c src/chip8_impl.c src/graphics.c src/sound.c src/keyboard.c src/debug.c -o bin/dy $(SDL) -g

emu_install: src/*.c src/*.h
	mkdir -p bin
	$(CC) $(WARN) -lSDL2_mixer src/chip8.c src/chip8_impl.c src/graphics.c src/sound.c src/keyboard.c src/debug.c -o bin/dy $(SDL)	
	chmod 755 bin/dy
	chmod 755 sound/edited_alarm.wav
	mkdir -p $$HOME/.config
	mkdir -p $$HOME/.config/dy-chip8
	mkdir -p $$HOME/.config/dy-chip8/sound
	sudo mkdir -p /usr/share/dy-chip8
	sudo cp sound/edited_alarm.wav /usr/share/dy-chip8/edited_alarm.wav
	sudo cp bin/dy /usr/bin/

emu_opti_install: src/*.c src/*.h
	mkdir -p bin
	$(CC) $(WARN) $(FLAGS) src/chip8.c src/chip8_impl.c src/graphics.c src/sound.c src/keyboard.c src/debug.c -o bin/dy $(SDL)	
	chmod 755 bin/dy
	chmod 755 sound/edited_alarm.wav
	sudo mkdir -p /usr/share/dy-chip8
	sudo cp sound/edited_alarm.wav /usr/share/dy-chip8/edited_alarm.wav
	sudo cp bin/dy /usr/bin/

clean:
	rm bin/*

clean_install:
	sudo rm -r /usr/share/dy-chip8
	sudo rm /usr/bin/dy
