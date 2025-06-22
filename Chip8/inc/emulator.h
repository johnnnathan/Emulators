#ifndef EMULATOR_H 
#define EMULATOR_H

extern const unsigned char chip8_fontset[80];



struct emulator {
    unsigned short opcode;
    unsigned char memory[4096];
    unsigned char graphics[64 * 32];
    unsigned char registers[16];

    unsigned short index;
    unsigned short program_counter;

    unsigned short stack[16];
    unsigned short stack_pointer;

    unsigned char keys[16];

    unsigned char delay_timer;
    unsigned char sound_timer;

};

void cycle(struct emulator *emu);
void init_emulator(struct emulator *emu);
void destroy_emulator(struct emulator *emu);
#endif // !EMULATOR_H

