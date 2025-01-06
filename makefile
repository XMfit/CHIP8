# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra

all: bin/emulator

bin/emulator: src/main.c src/chip8.c
	@if [ ! -d bin ]; then mkdir bin; fi
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf bin/*

