#include "../include/chip8.h"
#include "../include/sdl_utils.h"
#include <unistd.h>

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

    // Initalize graphics 
    if (init_display()) {
        fprintf(stderr, "Error intializing SDL\n");
        exit(EXIT_FAILURE);
    }
    printf("SDL initalized\n");

    printf("Starting emulation cycle\n");
    while (!quit_signal) {
        emulation_cycle();
        sdl_ehandler(keypad);
        if (df) {
            sdl_draw(display);
        }
        usleep(1000);
    }

    cleanup_sdl();
    printf("Exiting program\n");
    return 0;
}

