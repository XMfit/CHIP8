#include "../include/chip8.h"
#include "../include/sdl_utils.h"
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Incorrect amount of arguments: ./main <rom>\n");
        return 1;
    }

    // Grab input to set flag
    char buffer[100];
    char *endptr;
    int flag;
    long temp;

    printf("Enter legacy flag: <1> for legacy <0> for modern: ");
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        fprintf(stderr, "Error reading input\n");
        exit(EXIT_FAILURE);
    }

    buffer[strcspn(buffer, "\n")] = '\0';

    errno = 0;
    temp = strtol(buffer, &endptr, 10);

    if (errno != 0 || *endptr != '\0' || temp > INT_MAX || temp < INT_MIN) {
        fprintf(stderr, "Invalid input. Please enter valid integer.\n");
        exit(EXIT_FAILURE);
    }

    flag = (int)temp;
    
    // Init chip and load rom into memory
    initChip(flag);
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
   
    // Seed random
    srand(time(NULL));
    printf("Starting emulation cycle\n");

    while (!quit_signal) {
        emulation_cycle();
        sdl_ehandler(keypad);
        if (df)
            sdl_draw(display);

        while (debugger_enabled) {
            //print_registers();
            sdl_ehandler(keypad);
            if (step_next) {
                step_next = 0;
                break;
            }
        }

        usleep(2000);
    }

    cleanup_sdl();
    printf("Exiting program\n");
    return 0;
}

