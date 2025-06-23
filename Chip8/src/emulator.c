#include "../inc/emulator.h"
#include "../inc/random.h"
#include <stdio.h>
#include <stdlib.h>

const unsigned char chip8_fontset[80] =
{ 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void init_emulator(struct emulator *emu) {
    for (int i = 0; i < 4096; i++) {
        emu->memory[i] = 0;
    }

    int fontset_size = sizeof(chip8_fontset) / sizeof(chip8_fontset[0]);
    for (int i = 0; i < fontset_size; i++) {
        emu->memory[i] = chip8_fontset[i];
    }

    for (int i = 0; i < 16; i++) {
        emu->registers[i] = 0;
    }

    emu->index = 0;
    emu->program_counter = 0x200;

    for (int i = 0; i < 64 * 32; i++) {
        emu->graphics[i] = 0;
    }

    for (int i = 0; i < 16; i++) {
        emu->stack[i] = 0;
    }
    emu->stack_pointer = 0;

    emu->delay_timer = 0;
    emu->sound_timer = 0;

    for (int i = 0; i < 16; i++) {
        emu->keys[i] = 0;
    }

    emu->opcode = 0;
}

void push(struct emulator *emu, unsigned short value){
    emu->stack[emu->stack_pointer] = value;
    emu->stack_pointer += 1;
    return;
}
unsigned short pop(struct emulator *emu){
    emu->stack_pointer -= 1;
    unsigned short value = emu->stack[emu->stack_pointer];
    return value;
}

void set_register(struct emulator *emu, int reg_idx, unsigned char val){
    emu->registers[reg_idx] = val;
}


void increment_pc(struct emulator *emu){
    emu->program_counter += 2;
}

void print_state(struct emulator *emu){
    printf(
        "PC: %04X | High byte: %02X | Low byte: %02X | Opcode: %04X\n",
        emu->program_counter,
        emu->memory[emu->program_counter],
        emu->memory[emu->program_counter + 1],
        (emu->memory[emu->program_counter] << 8) | emu->memory[emu->program_counter + 1]
    );
}

void cycle(struct emulator *emu){

    emu->opcode = emu->memory[emu->program_counter] << 8 | emu->memory[emu->program_counter + 1];
    unsigned char operation = emu->opcode >> 12;
    unsigned short operand = emu->opcode & 0x0fff;

    switch (operation) {
        case 0:
            if (emu->opcode == 0x00E0){
                int len = sizeof(emu->graphics) / sizeof(emu->graphics[0]);
                for (int i = 0; i < len; i++){
                    emu->graphics[i] = 0;
                }
            }
            else if (emu->opcode == 0x00EE){
                unsigned int value = pop(emu);
                emu->program_counter = value;
            }
            increment_pc(emu);
            break;
        case 1:
            emu->program_counter = operand;
            break;
        case 2:
            emu->stack[emu->stack_pointer] = emu->program_counter;
            push(emu, emu->program_counter);
            emu->program_counter = operand;
            increment_pc(emu);
            break;
        case 3:
            if (emu->registers[(emu->opcode & 0x0f00) >> 8] == (emu->opcode & 0xff)){
                increment_pc(emu);
            }
            increment_pc(emu);
            break;
            
        case 4:
            if (emu->registers[(emu->opcode & 0x0f00) >> 8] != (emu->opcode & 0xff)){
                increment_pc(emu);
            }
            increment_pc(emu);
            break;
        case 5:
            if (emu->registers[(emu->opcode & 0x0f00) >> 8] == emu->registers[(emu->opcode & 0x00f0) >> 4]){
                increment_pc(emu);
            }
            increment_pc(emu);
            break;
        case 6:
            set_register(emu, (emu->opcode & 0xf00) >> 8, emu->opcode & 0xff);
            increment_pc(emu);
            break;
        case 7:{
            int byte_val =(emu->opcode & 0xff); 
            int reg_val = (emu->registers[emu->opcode & 0xf00] >> 8);
            int sum =  byte_val + reg_val;
            set_register(emu, (emu->opcode & 0xf00) >> 8,sum );
            increment_pc(emu);
            break;
        }
            
        case 8: {

            unsigned char r1_idx = (emu->opcode & 0x0f00) >> 8;
            unsigned char r2_idx = (emu->opcode & 0x00f0) >> 4;
            unsigned char r1_val = emu->registers[r1_idx];
            unsigned char r2_val = emu->registers[r2_idx];
            unsigned short val; 
            switch (emu->opcode & 0xf) {
                case 0:
                    set_register(emu, r1_idx, r2_val);
                    break;
                case 1:
                    val = r1_val | r2_val;
                    set_register(emu, r1_idx, r2_val);
                    break;
                case 2:
                    val = r1_val & r2_val;
                    set_register(emu, r1_idx, r2_val);
                    break;
                case 3:
                    val = r1_val ^ r2_val;
                    set_register(emu, r1_idx, r2_val);
                    break;
                case 4:
                    val = (r1_val + r2_val) & 0xff;
                    emu->registers[0xf] = (val & 0b100000000) >> 8;
                    set_register(emu, r1_idx, r2_val);
                    break;
                case 5:
                    val = (r1_val - r2_val) & 0xff;
                    if (r1_val > r2_val){
                        set_register(emu, 0xf, 1);
                    }else{
                        set_register(emu, 0xf, 0);
                    }
                    break;
                case 6:
                    if (r1_val & 0b1){
                        set_register(emu, 0xf, 1);
                    }else{
                        set_register(emu, 0xf, 0);
                    }
                    val = r1_val >> 1;
                    break;
                case 7:
                    val = (r2_val - r1_val) & 0xff;
                    if (r2_val > r1_val){
                        set_register(emu, 0xf, 1);
                    }else{
                        set_register(emu, 0xf, 0);
                    }
                    break;
                case 0xE:
                    if (r1_val & 0b100000000){
                        set_register(emu, 0xf, 1);
                    }else{
                        set_register(emu, 0xf, 0);
                    }
                    val = r1_val << 1;
                    break;
            }
            increment_pc(emu);
            break;
        }
        case 9:{
            int reg_1 = emu->registers[(emu->opcode & 0x0f00) >> 8];
            int reg_2 = emu->registers[(emu->opcode & 0xf0) >> 4];
            if (reg_1 != reg_2){
                increment_pc(emu);
            }
            increment_pc(emu);
            break;
        }
        case 0xA:
            emu->index = emu->opcode & 0xfff;
            increment_pc(emu);
            break;
        case 0xB:
            emu->program_counter = (emu->opcode & 0xfff) + emu->registers[0];
            break;
        case 0xC:
            set_register(emu, (emu->opcode & 0x0f00) >> 8, get_random_value() & (emu->opcode & 0xff));
            increment_pc(emu);
            break;
        case 0xE:{
            unsigned char reg = (emu->opcode & 0xf00) >> 8;
            unsigned char key = emu->opcode & 0xff;
            
            if (key == 0x9E){
                if (emu->keys[emu->registers[reg]] == 1){
                    increment_pc(emu);
                }
            }else if (key == 0xA1){
                if (emu->keys[emu->registers[reg]] != 1){
                    increment_pc(emu);
                }
            }
            increment_pc(emu);
            break;
        }

        case 0xD:
            set_register(emu, 0xF, 0);

            unsigned char r1_idx = (emu->opcode & 0x0F00) >> 8;
            unsigned char r2_idx = (emu->opcode & 0x00F0) >> 4;
            unsigned char n = emu->opcode & 0x000F;

            unsigned char x_pos = emu->registers[r1_idx];
            unsigned char y_pos = emu->registers[r2_idx];

            for (unsigned char y = 0; y < n; y++) {
                unsigned char pixel = emu->memory[emu->index + y];
                for (unsigned char x = 0; x < 8; x++) {
                    if ((pixel & (0x80 >> x)) != 0) {
                        int x_coord = (x_pos + x) % 64;
                        int y_coord = (y_pos + y) % 32;
                        int pixel_idx = x_coord + y_coord * 64;

                        emu->graphics[pixel_idx] ^= 1;  // Toggle pixel
                        if (emu->graphics[pixel_idx] == 0) {
                            set_register(emu, 0xF, 1); // Collision detected
                        }
                    }
                }
            }
            increment_pc(emu);
            break;
        case 0xF:{
            unsigned char reg = (emu->opcode & 0xf00) >> 8;
            switch (emu->opcode & 0xff) {
                case 0x07:
                    emu->registers[reg] = emu->delay_timer;
                    break;

                case 0x0A:{
                    int key_pressed = 0;
                    int size_keys = sizeof(emu->keys) / sizeof(emu->keys[0]);

                    for (int i = 0 ; i < size_keys; i++){
                        if (emu->keys[i] != 0){
                            emu->registers[reg] = i;
                            key_pressed = 1;
                            break;
                        }
                    }
                    if (!key_pressed){
                        increment_pc(emu);
                        return;
                    }
                    break;
                }
                case 0x15:
                    emu->delay_timer = emu->registers[reg];
                    break;
                case 0x18:
                    emu->sound_timer = emu->registers[reg];
                    break;
                case 0x1E:
                    emu->index += emu->registers[reg];
                    break;
                case 0x29:
                    if (emu->registers[reg] < 16){
                        emu->index = emu->registers[reg] * 0x5;
                    }
                    break;
                case 0x33:
                    emu->memory[emu->index + 0] = emu->registers[reg] / 100;
                    emu->memory[emu->index + 1] = (emu->registers[reg] / 10) % 10;
                    emu->memory[emu->index + 2] = emu->registers[reg] % 10;
                    break;
                case 0x55:{
                    int i = 0;
                    while (i <= reg){
                        emu->memory[emu->index + i] = emu->registers[i];
                        i++;
                    }
                }
                case 0x65:{
                    int i = 0;
                    while (i <= reg){
                        emu->registers[i] = emu->memory[emu->index + i];
                        i++;
                    }
                }

            }
            increment_pc(emu);
            break;

        }
        default:{
            printf("Error on Opcode: %04X\n", emu->opcode);
        }
    }
}

