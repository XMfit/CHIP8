#ifndef CHIP8_H
#define CHIP8_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

extern int legacy_flag;

extern uint8_t memory[4096];
extern uint16_t pc;
extern uint16_t I;

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

extern uint8_t registers[NUM_OF_REGISTERS]; // 16 8-bit registers
extern uint8_t timer_registers[NUM_OF_TIMERS];       // 2 8-bit timers

// Stack: 16 two-byte entries. Stack may be limited or unlimited
extern uint16_t stack[16];
extern uint16_t sp;

// Flags
extern uint8_t df; 
extern uint8_t sf;

// Keypad
extern uint8_t keypad[16];

// Display: 64 px * 32px. Each pixel can be on or off and is a boolean value
extern uint8_t display[64 * 32];

// Function Declarations
void initChip(int);
int loadRom(char *);
void emulation_cycle();

#endif

