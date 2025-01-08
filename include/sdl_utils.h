#ifndef SDL_UTILS_H
#define SDL_UTILS_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <stdint.h>
#include <stdio.h>

extern int quit_signal;

int init_display();
void sdl_ehandler(uint8_t *);
void sdl_draw(uint8_t *);
void cleanup_sdl();


#endif

