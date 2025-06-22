#include "../inc/screen.h"

int init_screen(SDL_Window **win, SDL_Renderer **rend, SDL_Texture **text) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return -1;
    }

    *win = SDL_CreateWindow("Chip-8",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            640, 320,
                            SDL_WINDOW_SHOWN);
    if (!*win) {
        printf("Error creating window: %s\n", SDL_GetError());
        return -1;
    }

    *rend = SDL_CreateRenderer(*win, -1, SDL_RENDERER_ACCELERATED);
    if (!*rend) {
        printf("Error creating renderer: %s\n", SDL_GetError());
        return -1;
    }

    *text = SDL_CreateTexture(*rend,
                              SDL_PIXELFORMAT_ABGR8888,
                              SDL_TEXTUREACCESS_STREAMING,
                              64, 32);
    if (!*text) {
        printf("Error creating texture: %s\n", SDL_GetError());
        return -1;
    }

    return 0;
}

int destroy_screen(SDL_Window *win, SDL_Renderer *rend, SDL_Texture *text) {
    if (text) SDL_DestroyTexture(text);
    if (rend) SDL_DestroyRenderer(rend);
    if (win) SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
