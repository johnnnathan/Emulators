#ifndef SCREEN_H
#define SCREEN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>


int init_screen(SDL_Window **win, SDL_Renderer **rend, SDL_Texture **text);
int destroy_screen(SDL_Window *win, SDL_Renderer *rend, SDL_Texture *text);

#endif
