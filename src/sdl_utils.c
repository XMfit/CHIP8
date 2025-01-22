#include "../include/sdl_utils.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <stdint.h>

SDL_Window *window;
SDL_Renderer * renderer; 

bool debugger_enabled;
bool step_next;

SDL_Scancode keymaps[16] = {
    SDL_SCANCODE_X, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, 
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_A,
    SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Z, SDL_SCANCODE_C,
    SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V,
};

int quit_signal = 0;

int init_display() {

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                              64 * 10, 32 * 10, 0);

    if (!window) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    return 0;
}

void sdl_ehandler(uint8_t *keypad) {
    SDL_Event event;
    static uint8_t prev_keypad[16] = {0}; // Track previous state of keys

    if (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                printf("Quit Emulator\n");
                quit_signal = 1;
                break;
            case SDL_KEYDOWN:
                for (int i = 0; i < 16; i++) {
                    if (event.key.keysym.scancode == keymaps[i]) {
                        // Only register the press if it wasn't already pressed
                        if (prev_keypad[i] == 0) {
                            keypad[i] = 1;
                            prev_keypad[i] = 1; // Update previous state
                        }
                        break;
                    }
                }
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    quit_signal = 1;
                }
                if (event.key.keysym.scancode == SDL_SCANCODE_RETURN) {
                    debugger_enabled = !debugger_enabled;
                    step_next = 0;
                }
                if (event.key.keysym.scancode == SDL_SCANCODE_RSHIFT) {
                    step_next = 1;    
                }
                break;
            case SDL_KEYUP:
                for (int i = 0; i < 16; i++) {
                    if (event.key.keysym.scancode == keymaps[i]) {
                        // Update states on key release
                        keypad[i] = 0;
                        prev_keypad[i] = 0; // Reset previous state
                        break;
                    }
                }
                break;

            default:
                break;
        }
    }
}

void sdl_draw(uint8_t *display) {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    // Set color to black
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // iterate through display 
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            if (display[x + (y * 64)]) {
                SDL_Rect rect;
                rect.x = x * 10; 
                rect.y = y * 10;
                rect.w = 10; 
                rect.h = 10;

                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void cleanup_sdl() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

