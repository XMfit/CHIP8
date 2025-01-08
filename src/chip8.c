#include "../include/chip8.h"
#include <stdint.h>

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

uint8_t df;
uint8_t sf;

uint8_t keypad[16]; 
uint8_t display[64 * 32]; 


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

void emulation_cycle() {
    // Set flags to 0 at start of draw cycle;
    df = 0;
    sf = 0; 

    uint16_t opcode = memory[pc] << 8 | memory[pc + 1];
   
    uint16_t Vx = (opcode & 0x0F00) >> 8;
    uint16_t Vy = (opcode & 0x00F0) >> 4; 

    // printf("%04x\n", opcode);

    // First nibble tells what type of operation it is 
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x00FF) {
                case 0xE0: // clear screen 
                    for (int i = 0; i < 64 * 32; i++)
                        display[i] = 0;
                    pc += 2;
                    printf("Cleared\n");
                    break;
                case 0xEE: // return from subroutine 
                    // code 
                    break;
                default: 
                    // Other 0x0XXX case 
                    break;
            }
            break;
        case 0x1000:
            // 0x1nnn -> JP Addr
            pc = (opcode & 0x0FFF); 
            break;
        case 0x2000:
            // 0x2nnn
            break;
        case 0x3000:
            // 0x3xkk
            break;
        case 0x4000:
            // 0x4xkk 
            break;
        case 0x5000:
            // 0x5xy0 
            break;
        case 0x6000:
            // 0x6xkk set register VX 
            registers[Vx] = (opcode & 0x00FF);
            pc += 2;
            break;
        case 0x7000:
            // 0x7xkk add value to register vx
            registers[Vx] += (opcode & 0x00FF); 
            pc += 2;
            break;
        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0:
                    // 0x8xy0
                    break;
                case 0x1:
                    // 0x8xy1
                    break;
                case 0x2:
                    // 0x8xy2
                    break;
                case 0x3:
                    // 0x8xy3
                    break;
                case 0x4:
                    // 0x8xy4
                    break;
                case 0x5:
                    // 0x8xy5  
                    break;
                case 0x6:
                    // 0x8xy6
                    break;
                case 0x7:
                    // 0x8xy7
                    break;
                case 0xE:
                    // 0x8xye
                    break;
            }
            break;
        case 0x9000:
            // 0x9xy0
            break;
        case 0xA000:
            // 0xAnnn set value of register I to nnn 
            I = (opcode & 0x0FFF);
            pc += 2;
            break;
        case 0xB000:
            // 0xBnnn
            break;
        case 0xC000:
            // 0xCxkk
            break;
        case 0xD000:
            // 0xDxyn DRW Vx, Vy, nibble 
            df = 1;

            uint16_t height = (opcode & 0x000F);
            uint16_t px;

            registers[VF] = 0;

            for (int yline = 0; yline < height; yline++) {
                px = memory[I + yline];
                for (int xline = 0; xline < 8; xline++) {
                    if ((px & (0x80 >> xline)) != 0) {
                        int screen_x = registers[Vx] + xline;
                        int screen_y = registers[Vy] + yline;
                        int pixel_index = screen_x + (screen_y * 64);
                        
                        if (display[pixel_index] == 1)
                            registers[VF] = 1;

                        display[pixel_index] ^= 1;
                    }
                }
            }
            pc += 2;
            break;
        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x9E:
                    break;
                case 0xA1:
                    break;
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x07:
                    break;
                case 0x0A:
                    break;
                case 0x15:
                    break;
                case 0x18:
                    break;
                case 0x1E:
                    break;
                case 0x29:
                    break;
                case 0x33:
                    break;
                case 0x55:
                    break;
                case 0x65:
                    break;
            }
            break;
    }
}

