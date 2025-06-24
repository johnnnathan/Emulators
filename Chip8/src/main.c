#include "../inc/screen.h"
#include "../inc/emulator.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <unistd.h>

// 60 HZ
#define HZt 5



int load_rom(struct emulator *emu, const char *filename) {
    FILE *fp = fopen(filename, "rb"); // 'rb' for binary mode
    if (fp == NULL) {
        printf("Could not open file: %s\n", filename);
        return 1;
    }

    // Seek to end to get size
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    printf("ROM File Size: %ld bytes\n", size);

    // Safety check
    if (size > (4096 - 0x200)) {
        printf("ROM too large to fit in memory!\n");
        fclose(fp);
        return 1;
    }

    // Read directly into memory
    size_t bytes_read = fread(&emu->memory[0x200], 1, size, fp);
    if (bytes_read != size) {
        printf("Failed to read entire ROM\n");
        fclose(fp);
        return 1;
    }

    printf("Loading ROM Succeeded!\n");
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
            bytes[index] = (emu->graphics[index] == 1) ? 0xFFFFFFFF : 0xFF000000;
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
        print_state(&emu);
        
        while (SDL_PollEvent(&e) > 0){
            switch (e.type){
                case SDL_QUIT:
                    alive = 0;
                    break;
                default:

            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        build_texture(&emu, texture);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_Delay(HZt);
    }


    destroy_screen(window, renderer, texture);
    return 0;
}

