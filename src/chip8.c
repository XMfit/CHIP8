#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>

// Memory: 4kb (4096 bytes)
uint8_t memory[4096];

// Special Registers 
uint16_t pc;            // Program counter 
uint16_t I;             // Points to memory locations 

// Timer Registers
typedef enum {
    dt, st,         // dt - delay timer, st - start timer
    NUM_OF_TIMERS
} TimerRegs;

// General Purpose Registers 
typedef enum {
    V0, V1, V2, V3, V4, V5, V6, V7, V8, V9,
    VA, VB, VC, VD, VE, VF,     // VF - flag register
    NUM_OF_REGISTERS
} Registers;

uint8_t registers[NUM_OF_REGISTERS]; // 16 8-bit registers 
uint8_t timers[NUM_OF_TIMERS];       // 2 8-bit timers

// Stack: 16 two-byte entries. Stack may be limited or unlimited 
uint16_t stack[16];
uint16_t sp;

// Keypad
uint8_t keypad[16];

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

// Display: 64 px * 32px. Each pixel can be on or off and is a boolean value
bool display[64 * 32];

void initChip() {
    memset(memory, 0, sizeof(memory));
    memset(stack, 0, sizeof(stack));
    memset(registers, 0, sizeof(registers));
    memset(timers, 0, sizeof(timers));
    memset(display, 0, 64 * 32);
    
    pc = 0x200;
    I = 0;
    sp = 0;

    memset(keypad, 0, sizeof(keypad));
    memset(display, 0, sizeof(display));
    // load fonts 
    memcpy(memory, fontset, sizeof(fontset));
}

int main(void) {
    printf("Hello World!\n");
    return 0;
}

