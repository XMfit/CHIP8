#include "../include/chip8.h"

// Font: 4px X 5px. Stored in memory from 000 - 1FF. Popular at 050 - 09F
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

uint8_t memory[4096];
uint16_t pc; 
uint16_t I;
uint8_t registers[NUM_OF_REGISTERS];

uint8_t timer_registers[NUM_OF_TIMERS];
uint16_t stack[16];
uint16_t sp;

uint8_t keypad[16]; 
bool display[64 * 32]; 


void initChip() {
    memset(memory, 0, sizeof(memory));
    memset(stack, 0, sizeof(stack));
    memset(registers, 0, sizeof(registers));
    memset(timer_registers, 0, sizeof(timer_registers));
    memset(display, 0, 64 * 32);
    
    pc = 0x200;
    I = 0;
    sp = 0;

    memset(keypad, 0, sizeof(keypad));
    memset(display, 0, sizeof(display));
    // load fonts 
    memcpy(memory + 0x50, fontset, sizeof(fontset));
    printf("Chip Initialized\n");
}

int loadRom(char *filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Error opening ROM file\n");
        return 1;
    }

    struct stat file_stat;              // Used to grab info on file
    stat(filename, &file_stat);         
    size_t fsize = file_stat.st_size;   // Grab size of rom

    // Read binary data into memory starting at address 0x200 
    size_t read_elements = fread(memory + 0x200, 1, sizeof(memory) - 0x200, f);
    
    fclose(f);

    // check read elements equals size of file in bytes
    if (read_elements != fsize) {
        fprintf(stderr, "Error: unable to load ROM into available memory");
        return 1;
    }
    return 0;
}

