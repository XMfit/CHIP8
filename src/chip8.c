#include "../include/chip8.h"
#include <stdint.h>
#include <stdarg.h>

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

int legacy_flag;

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


#ifndef DEBUG 
#define DEBUG 0
#endif

void debug_ops(const char *fmt, ...) {
    if (DEBUG) {
        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
    }
}

void initChip(int flag) {
    legacy_flag = flag; 
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
                    debug_ops("Opcode debug: 0x%X:\n", opcode);
                    for (int i = 0; i < 64 * 32; i++)
                        display[i] = 0;
                    pc += 2;
                    break;
                case 0xEE: // return from subroutine
                    debug_ops("Opcode debug: 0x%X:\n", opcode);
                    pc = stack[sp];
                    sp--;
                    pc += 2;
                    break;
                default: 
                    // Other 0x0XXX case 
                    break;
            }
            break;
        case 0x1000:
            // 0x1nnn -> JP Addr
            debug_ops("Opcode debug: 0x%X:\n", opcode);
            pc = (opcode & 0x0FFF); 
            break;
        case 0x2000:
            // 0x2nnn Call addr
            debug_ops("Opcode debug: 0x%X:\n", opcode);
            sp++; 
            stack[sp] = pc; 
            pc = (opcode & 0x0FFF);
            break;
        case 0x3000:
            // 0x3xkk SE registers[Vx], byte 
            debug_ops("Opcode debug: 0x%X:\n", opcode);
            if (registers[Vx] == (opcode & 0x00FF))
                pc += 2;
            pc += 2;
            break;
        case 0x4000:
            // 0x4xkk SNE registers[Vx], byte
            debug_ops("Opcode debug: 0x%X:\n", opcode);
            if (registers[Vx] != (opcode & 0x00FF))
                pc += 2;
            pc += 2;
            break;
        case 0x5000:
            // 0x5xy0 SE registers[Vx], registers[Vy]
            debug_ops("Opcode debug: 0x%X:\n", opcode);
            if (registers[Vx] == registers[Vy])
                pc += 2;
            pc += 2;
            break;
        case 0x6000:
            // 0x6xkk set register registers[Vx]
            debug_ops("Opcode debug: 0x%X:\n", opcode);
            registers[Vx] = (opcode & 0x00FF);
            pc += 2;
            break;
        case 0x7000:
            // 0x7xkk add value to register registers[Vx]
            debug_ops("Opcode debug: 0x%X:\n", opcode);
            registers[Vx] += (opcode & 0x00FF); 
            pc += 2;
            break;
        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0:
                    // 0x8xy0 LD registers[Vx], registers[Vy]
                    debug_ops("Opcode debug: 0x%X:\n", opcode);
                    registers[Vx] = registers[Vy];
                    pc += 2;
                    break;
                case 0x1:
                    // 0x8xy1 OR registers[Vx], registers[Vy]
                    debug_ops("Opcode debug: 0x%X:\n", opcode);
                    registers[Vx] = (registers[Vx] | registers[Vy]);
                    pc += 2;
                    break;
                case 0x2:
                    // 0x8xy2 AND registers[Vx], registers[Vy]
                    debug_ops("Opcode debug: 0x%X:\n", opcode);
                    registers[Vx] = (registers[Vx] & registers[Vy]);
                    pc += 2;
                    break;
                case 0x3:
                    // 0x8xy3 XOR, registers[Vx], registers[Vy]
                    debug_ops("Opcode debug: 0x%X:\n", opcode);
                    registers[Vx] = (registers[Vx] ^ registers[Vy]);
                    pc += 2;
                    break;
                case 0x4:
                    // 0x8xy4 ADD registers[Vx], registers[Vy]
                    debug_ops("Opcode debug: 0x%X:\n", opcode);
                    if (registers[Vx] + registers[Vy] > 0xFF)
                        registers[VF] = 1;
                    else
                        registers[VF] = 0;
                    registers[Vx] += registers[Vy];
                    pc += 2;
                    break;
                case 0x5:
                    // 0x8xy5  SUB registers[Vx], registers[Vy] 
                    debug_ops("Opcode debug: 0x%X:\n", opcode);
                    if (registers[Vx] > registers[Vy])
                        registers[VF] = 1;
                    else
                        registers[VF] = 0;
                    registers[Vx] = registers[Vx] - registers[Vy];
                    pc += 2;
                    break;
                case 0x6:
                    // 0x8xy6 SHR registers[Vx] {, registers[Vy]}
                    debug_ops("Opcode debug: 0x%X:\n", opcode);
                    if (legacy_flag) {
                        registers[Vx] = registers[Vy];
                    }
                    registers[0xF] = registers[Vx] & 0x1;
                    registers[Vx] >>= 1;
                    pc += 2;
                    break;
                case 0x7:
                    // 0x8xy7 SUBN registers[Vx], registers[Vy]
                    debug_ops("Opcode debug: 0x%X:\n", opcode);
                    if (registers[Vy] > registers[Vx])
                        registers[VF] = 1;
                    else
                        registers[VF] = 0;
                    registers[Vx] = registers[Vy] - registers[Vx];
                    pc += 2;
                    break;
                case 0xE:
                    // 0x8xye SHL registers[Vx] {, registers[Vy]}
                    debug_ops("Opcode debug: 0x%X:\n", opcode);
                    if (legacy_flag) {
                        registers[Vx] = registers[Vy];
                    }
                    registers[VF] = (registers[Vx] & 0x80) >> 7;
                    registers[Vx] <<= 1;
                    pc += 2;
                    break;
            }
            break;
        case 0x9000:
            // 0x9xy0 SNE registers[Vx], registers[Vy]
            debug_ops("Opcode debug: 0x%X:\n", opcode);
            if (registers[Vx] != registers[Vy])
                pc += 2;
            pc += 2;
            break;
        case 0xA000:
            // 0xAnnn set value of register I to nnn 
            debug_ops("Opcode debug: 0x%X:\n", opcode);
            I = (opcode & 0x0FFF);
            pc += 2;
            break;
        case 0xB000:
            // 0xBnnn JP, V0, addr 
            debug_ops("Opcode debug: 0x%X:\n", opcode);
            pc = (opcode & 0x0FFF) + registers[V0]; 
            break;
        case 0xC000:
            // 0xCxkk RND, registers[Vx], byte
            debug_ops("Opcode debug: 0x%X:\n", opcode);
            registers[Vx] = (rand() % 256) & (opcode & 0x00FF);
            pc += 2;
            break;
        case 0xD000:
            // 0xDxyn DRW registers[Vx], registers[Vy], nibble 
            debug_ops("Opcode debug: 0x%X:\n", opcode);
            df = 1;

            uint16_t height = (opcode & 0x000F);
            uint16_t px;

            registers[VF] = 0;

            for (int yline = 0; yline < height; yline++) {
                px = memory[I + yline];
                for (int xline = 0; xline < 8; xline++) {
                    if ((px & (0x80 >> xline)) != 0) {
                        int screen_x = (registers[Vx] + xline) % 64;
                        int screen_y = (registers[Vy] + yline) % 32;
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
                    // Ex9E Skip registers[Vx]
                    debug_ops("Opcode debug: 0x%X:\n", opcode);
                    if (keypad[registers[Vx]])
                        pc += 2;
                    pc += 2;
                    break;
                case 0xA1:
                    // ExAI SKNP registers[Vx]
                    debug_ops("Opcode debug: 0x%X:\n", opcode);
                    if (!keypad[registers[Vx]])
                        pc += 2;
                    pc += 2;
                    break;
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x07:
                    // Fx07 Ld registers[Vx], DT
                    debug_ops("Opcode debug: 0x%X:\n", opcode);
                    registers[Vx] = timer_registers[dt];
                    pc += 2;
                    break;
                case 0x0A:
                    // Fx0A LD registers[Vx], K
                    debug_ops("Opcode debug: 0x%X:\n", opcode);
                    for (int i = 0; i < 16; i++) {
                        if(keypad[i]) {
                            registers[Vx] = i;
                            pc += 2;
                            break;
                        }
                    }
                    break;
                case 0x15:
                    // Fx15 LD DT, registers[Vx]
                    debug_ops("Opcode debug: 0x%X:\n", opcode);
                    timer_registers[dt] = registers[Vx];
                    pc += 2;
                    break;
                case 0x18:
                    // Fx18 LD St, registers[Vx]
                    debug_ops("Opcode debug: 0x%X:\n", opcode);
                    timer_registers[st] = registers[Vx];
                    pc += 2;
                    break;
                case 0x1E:
                    // Fx1E ADD I, registers[Vx] 
                    debug_ops("Opcode debug: 0x%X:\n", opcode);
                    I = I + registers[Vx];
                    pc += 2;
                    break;
                case 0x29:
                    // Fx29 LD F, registers[Vx]
                    debug_ops("Opcode debug: 0x%X:\n", opcode);
                    I = registers[Vx] * 5;
                    pc += 2;
                    break;
                case 0x33:
                    // Fx33 LD B, registers[Vx]
                    debug_ops("Opcode debug: 0x%X:\n", opcode);
                    memory[I] = registers[Vx] / 100;
                    memory[I + 1] = registers[Vx] / 10;
                    memory[I + 2] = registers[Vx] % 10;
                    pc += 2;
                    break;
                case 0x55:
                    // Fx55 LD [i] registers[Vx]
                    debug_ops("Opcode debug: 0x%X:\n", opcode);
                    for (int i = 0; i <= Vx; i++)
                        memory[I + i] = registers[i];
                    pc += 2;
                    break;
                case 0x65:
                    // Fx65 LD registers[Vx], [I]
                    debug_ops("Opcode debug: 0x%X:\n", opcode);
                    for (int i = 0; i <= Vx; i++)
                        registers[i] = memory[I + i];
                    pc += 2;
                    break;
            }
            break;
    }

    if (timer_registers[dt] > 0)
        timer_registers[dt] -= 1;
    if (timer_registers[st] > 0) {
        // sound 
        timer_registers[st] -= 1;
    }

}

