#include "../inc/screen.h"
#include "../inc/emulator.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <unistd.h>

// 60 HZ
#define HZt 16


int load_rom(struct emulator *emu, const char *filename){
    FILE *fp = fopen(filename, "r"); 
    char c;
    int i = 0;;

    if (fp == NULL){
        printf("Could not open file");
        return 1;
    }

    while ((c = fgetc(fp)) != EOF){
        emu->memory[0x200 + i] = c;
        i += 1;
    }
    fclose(fp);
    return 0;

}

void build_texture(struct emulator *emu, SDL_Texture *texture) {
    void *pixels = NULL;
    int pitch = 0;

    SDL_LockTexture(texture, NULL, &pixels, &pitch);

    unsigned int *bytes = (unsigned int *)pixels;

    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            int index = y * 64 + x;
            bytes[index] = (emu->graphics[index] == 1) ? 0xFFFFFFFF : 0x000000FF;
        }
    }

    SDL_UnlockTexture(texture);
}


int main(int argc, char *argv[])
{

    setvbuf(stdout, NULL, _IONBF, 0);

    struct emulator emu;
    init_emulator(&emu);


    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;
    SDL_Event e;
    init_screen(&window, &renderer, &texture);
    int alive = 1;

    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    printf("Filename: %s\n", filename);

    load_rom(&emu,filename);

    while (alive){
        cycle(&emu);
        unsigned short opcode = emu.opcode;
        unsigned short reversed = (opcode >> 8) | (opcode << 8);

        printf("Opcode: %04X\n", reversed);
        while (SDL_PollEvent(&e) > 0){
            switch (e.type){
                case SDL_QUIT:
                    alive = 0;
                    break;
                default:

            }
        }
        SDL_RenderClear(renderer);

        build_texture(&emu, texture);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_Delay(HZt);
    }


    destroy_screen(window, renderer, texture);
    return 0;
}

