#include "../include/chip8.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Incorrect amount of arguments: ./main <rom>\n");
        return 1;
    }
    
    // Init chip and load rom into memory

    initChip();
    if (loadRom(argv[1])) {
        fprintf(stderr, "Error loading ROM file\n");
        exit(EXIT_FAILURE);
    }
    
    printf("ROM loaded Successfully\n");
    return 0;
}

