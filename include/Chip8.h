#pragma once
#include <string>
#include "constants.h"


class Chip8 {
    private:
        unsigned char stack[STACK_BYTES];
        unsigned char main_memory[MEMORY_BYTES];
        unsigned char registers[16];
        unsigned char sound_timer;
        unsigned char delay_timer;
        unsigned char stack_pointer;
        unsigned short program_counter;
        unsigned short index_register;

        unsigned char key_register = -1;

    public:
        bool frame_buffer[PIXELS_WIDTH * PIXELS_HEIGHT];
        bool draw_flag = 1;

    public:
        Chip8();
        void load_rom_to_memory(std::string);
        void update_timers();
        void complete_one_instruction();

        inline bool* get_frame_buffer() { return frame_buffer; }

    private:
        void initialize_main_memory();
        void clear_frame_buffer();

        unsigned short pop_stack();
        void push_stack(unsigned short);

        bool wait_for_key();

        void detail_instruction(std::string);
};
