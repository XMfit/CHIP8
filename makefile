# Compiler and flags
CC = gcc
CFLAGS = -Iinclude -Wall -Wextra
LDFLAGS = -lSDL2

all: bin/emulator

bin/emulator: src/main.c src/chip8.c src/sdl_utils.c
	@if [ ! -d bin ]; then mkdir bin; fi
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -DDEBUG=1 -o $@

clean:
	rm -rf bin/*

