#include "Chip8.h"
#include "constants.h"
#include <SFML/Window/Keyboard.hpp>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <ios>

Chip8::Chip8() {
    initialize_main_memory();
    clear_frame_buffer();

    stack_pointer = 0;
}

void Chip8::initialize_main_memory() {
    for (int i = 0; i < MEMORY_BYTES; i++) {
        main_memory[i] = 0; 
    }

    // initialize all of the digit sprites
    for (int i = 0; i < PRESET_DIGIT_SPRITES_SIZE; i++) {
        main_memory[i] = PRESET_DIGIT_SPRITES[i];
    }
}

void Chip8::load_rom_to_memory(std::string rom_file_name) {
    unsigned char* rom_start_pointer = &main_memory[0x200];
    
    std::ifstream myFile(rom_file_name, std::ios::binary);

    char ch;
    while (myFile.get(ch)) {
        *rom_start_pointer = (unsigned char)ch;
        rom_start_pointer++;
    }

    program_counter = 0x200;


    // for (int i = 0x200; i < 4096; i++) {
    //     printf("0x%x ", main_memory[i]);
    //     if (i % 0x100 == 0 && i != 0x200) {
    //         printf("\n");
    //         printf("%d: ", i);
    //     }
    // }

}

void Chip8::clear_frame_buffer() {
    for (int r = 0; r < PIXELS_HEIGHT; r++) {
        for (int c = 0; c < PIXELS_WIDTH; c++) {
            frame_buffer[r * PIXELS_WIDTH + c] = 0;
        }
    }
}

// This happens 1 per second
void Chip8::update_timers() {
    if (sound_timer > 0) {
        sound_timer--;
    }
    
    if (delay_timer > 0) {
        delay_timer--;
    }
}

unsigned short instruction;

void Chip8::detail_instruction(std::string instruction_description) {
    const int GAP = 10;
    if (DEBUG) {
        printf("0x%x: 0x%x - %s\n", program_counter, instruction, instruction_description.c_str());
        // print registers
        printf("Registers:\n");
        for (int i = 0; i <= 0xFu; i++) {
            printf("V%x: 0x%x ", i, registers[i]);
            if (i % 6 == 0) {
                printf("\n");
            }
        }
        printf("\n");
        // print index_register
        printf("Index Register: %x\n", index_register);
        // print frame_buffer
        printf("Frame Buffer:\n");
        for(int r = 0; r < PIXELS_HEIGHT; r++) {
            for (int c = 0; c < PIXELS_WIDTH; c++) {
                printf("%d ", frame_buffer[r * PIXELS_WIDTH + c]);
            }
            printf("\n");
        }
        // print main memory around index register
        printf("Main memory:\n");
        printf("0x%x: ", index_register - GAP);
        for (int i = index_register - GAP; i <= index_register + GAP; i++) {
            printf("0x%x ", main_memory[i]);
        }
        printf("\n\n\n");
    }
}

// This happens multiple times per second and it can be controlled by the user
// for the overall speed of the game
void Chip8::complete_one_instruction() {
    if (wait_for_key()) {
        printf("Waiting for key repeatedly\n");
        return;
    }

    // fetch the instruction
    instruction = main_memory[program_counter] << 8;
    instruction |= main_memory[program_counter + 1];

    // printf("The current instruction is 0x%x at program counter 0x%x\n", instruction, program_counter);

    program_counter += 2;

    // decode instruction
    unsigned short X = (instruction & 0x0F00u) >> 8;
    unsigned short Y = (instruction & 0x00F0u) >> 4;
    unsigned short N = (instruction & 0x000Fu);
    unsigned short NN = (instruction & 0x00FFu);
    unsigned short NNN = (instruction & 0x0FFFu);

    unsigned int in_between = 0;
    switch ((instruction & 0xF000u) >> 12) {
        case 0x0u:
            if(instruction == 0x00E0u) {
                // clear screen
                clear_frame_buffer();
                draw_flag = 1;
                detail_instruction("Clear Frame");
            } else if (instruction == 0x00EEu) {
                // return
                program_counter = pop_stack();
                detail_instruction("Return");
            } else {
                printf("INVALID INSTRUCTION 0x%x\n", instruction);
            }
            break;
        case 0x1u:
            // jump to NNN
            program_counter = NNN;
            detail_instruction("Jump to NNN");
            break;
        case 0x2u:
            push_stack(program_counter);
            program_counter = NNN;
            detail_instruction("Call Function NNN");
            break;
        case 0x3u:
            // skip next instruction if Vx = NN
            if (registers[X] == NN) {
                program_counter += 2;
            }
            detail_instruction("Skip next instruction if Vx = NN");
            break;
        case 0x4u:
            // skip next instruction if Vx != NN
            if (registers[X] != NN) {
                program_counter += 2;
            }
            detail_instruction("Skip next instruction if Vx != NN");
            break;
        case 0x5u:
            // skip next instruction if Vx = Vy
            if (registers[X] == registers[Y]) {
                program_counter += 2;
            }
            detail_instruction("Skip next instruction if Vx = Vy");
            break;
        case 0x6u:
            // set Vx = NN
            registers[X] = NN;
            detail_instruction("Set Vx to NN");
            break;
        case 0x7u:
            // increment Vx by NN
            registers[X] += NN;
            detail_instruction("Increment Vx by NN");
            break;
        case 0x8u:
            switch(instruction & 0xFu) {
                case 0x0u:
                    // Set Vx = Vy
                    registers[X] = registers[Y];
                    detail_instruction("Set Vx to Vy");
                    break;
                case 0x1u:
                    // Set Vx = Vy OR Vx
                    registers[X] = registers[X] | registers[Y];
                    detail_instruction("Set Vx to Vy OR Vx");
                    break;
                case 0x2u:
                    // Set Vx = Vx AND Vy
                    registers[X] = registers[X] & registers[Y];
                    detail_instruction("Set Vx to Vy AND Vx");
                    break;
                case 0x3u:
                    // Set Vx = Vx XOR Vy
                    registers[X] = registers[X] ^ registers[Y];
                    detail_instruction("Set Vx to Vy XOR Vx");
                    break;
                case 0x4u:
                    // Set Vx = Vx + Vy and have VF = carry
                    in_between = (unsigned int)registers[X] + (unsigned int)registers[Y];
                    registers[X] = (unsigned char)(in_between & 0xFFu);
                    registers[0xF] = (unsigned char)((in_between >> 8) > 0);
                    detail_instruction("Set Vx to Vy + Vx");
                    break;
                case 0x5u:
                    // Set Vx = Vx - Vy
                    registers[0xF] = 1;
                    if (registers[X] <= registers[Y]) {
                        registers[0xF] = 0;
                    }
                    registers[X] -= registers[Y];
                    detail_instruction("Set Vx to Vx - Vy");
                    break;
                case 0x6u:
                    // Set Vx = Vx / 2
                    registers[0xF] = registers[X] % 2;
                    registers[X] = registers[X] / 2;
                    detail_instruction("Set Vx to Vx / 2");
                    break;
                case 0x7u:
                    // Set Vx = Vy - Vx
                    registers[0xF] = 1;
                    if (registers[Y] <= registers[X]) {
                        registers[0xF] = 0;
                    }
                    registers[Y] -= registers[X];
                    detail_instruction("Set Vx to Vy - Vx");
                    break;
                case 0xEu:
                    // Set Vx = Vx * 2
                    registers[0xF] = registers[X] >> 7;
                    registers[X] = registers[X] * 2;
                    detail_instruction("Set Vx to Vx * 2");
                    break;
                default:
                    printf("INVALID INSTRUCTION 0x%x\n", instruction);
                    break;
            }
            break;
        case 0x9u:
            // skip the next instruction if Vx != Vy
            if (registers[X] != registers[Y]) {
                program_counter += 2;
            }
            detail_instruction("Skip next instruction if Vx != Vy");
            break;
        case 0xAu:
            // set I = NNN
            index_register = NNN;
            detail_instruction("Set index register to NNN");
            break;
        case 0xBu:
            // jump to NNN + V0
            program_counter = NNN + registers[0];
            detail_instruction("Set program counter to NNN + V0(Jump)");
            break;
        case 0xCu:
            // set Vx = random byte & kk
            registers[X] = (rand() % 256) & NN;
            detail_instruction("Set Vx to a random byte AND NN");
            break;
        case 0xDu:
            // draw a sprite onto the screen from memory address I at (Vx, Vy)
            registers[0xF] = 0;
            for (int r = 0; r < N; r++) {
                unsigned char sprite = main_memory[index_register + r];
                for (int c = 0; c < 8; c++) {
                    unsigned char new_pixel_value = (sprite >> (7 - c)) & 1;
                    bool* new_pixel_location = &frame_buffer[((registers[Y] + r) % PIXELS_HEIGHT) * PIXELS_WIDTH + ((registers[X] + c) % PIXELS_WIDTH)];
                    if (*new_pixel_location == 1 && new_pixel_value == 1) {
                        registers[0xF] = 1;
                    }
                    *new_pixel_location ^= new_pixel_value;
                }
            }
            draw_flag = 1;
            detail_instruction("Draw a sprite at (Vx, Vy)");
            break;
        case 0xEu:
            if((instruction & 0xFFu) == 0x9Eu) {
                // Skip next instruction if key at Vx is pressed
                if (sf::Keyboard::isKeyPressed(CHIP_8_KEYS.at(registers[X]))) {
                    program_counter += 2;
                }
                detail_instruction("Skip next instruction if key is pressed");
            } else if ((instruction & 0xFFu) == 0xA1u) {
                // Skip next instruction if key at Vx is not pressed
                if (!sf::Keyboard::isKeyPressed(CHIP_8_KEYS.at(registers[X]))) {
                    program_counter += 2;
                }
                detail_instruction("Skip next instruction if key is not pressed");
            } else {
                printf("INVALID INSTRUCTION 0x%x\n", instruction);
            }
            break;
        case 0xFu:
            switch(instruction & 0xFFu) {
                case 0x07u:
                    // Set Vx to the delay timer value
                    registers[X] = delay_timer;
                    detail_instruction("Set Vx to the delay timer value");
                    break;
                case 0x0Au:
                    // Wait for a key press and store the resulting key in Vx
                    key_register = X;
                    detail_instruction("Wait for a key press");
                    break;
                case 0x15u:
                    // set delay timer to Vx
                    delay_timer = registers[X];
                    detail_instruction("Set delay timer to Vx");
                    break;
                case 0x18u:
                    // set sound timer to Vx
                    sound_timer = registers[X];
                    detail_instruction("Set sound timer to Vx");
                    break;
                case 0x1Eu:
                    // Set I = I + Vx
                    index_register += registers[X];
                    detail_instruction("Set index timer to Vx + previous index timer");
                    break;
                case 0x29u:
                    // Set I to the location of the digit sprite for Vx
                    index_register = registers[X] * 5;
                    detail_instruction("Set index register to the location of digit sprite for Vx");
                    break;
                case 0x33u:
                    // Store the BCD representation of Vx in memory I, I + 1, I + 2
                    main_memory[index_register] = (registers[X] / 100) % 10;
                    main_memory[index_register + 1] = (registers[X] / 10) % 10;
                    main_memory[index_register + 2] = registers[X] % 10;
                    detail_instruction("Store the BCD representation of Vx in memory");
                    break;
                case 0x55u:
                    // Store V0 to Vx in memory addresses I, I + x, and then makes I be I + x + 1
                    for (int i = 0; i <= X; i++) {
                        main_memory[index_register + i] = registers[i];
                    }
                    index_register += X + 1;
                    detail_instruction("Store V0 to Vx in memory and update the index register to be the next spot");
                    break;
                case 0x65u:
                    // Set V0 to Vx with values in memory addresses I, I + x and then makes I be I + x + 1
                    for (int i = 0; i <= X; i++) {
                        registers[i] = main_memory[index_register + i];
                    }
                    index_register += X + 1;
                    detail_instruction("Set V0 to Vx with memory at index register");
                    break;
                default:
                    printf("INVALID INSTRUCTION 0x%x\n", instruction);
            }
            break;
        default:
            printf("INVALID INSTRUCTION 0x%x\n", instruction);
            break;
    }
}

unsigned short Chip8::pop_stack() {
    if (stack_pointer <= 1) {
        printf("[ERROR] There is nothing on the stack to pop from");
        return 0;
    }
    unsigned short top_stack = ((unsigned short)(stack[stack_pointer - 2]) << 8) + stack[stack_pointer - 1];
    stack_pointer -= 2;
    return top_stack;
}

void Chip8::push_stack(unsigned short val) {
    stack[stack_pointer] = (unsigned char)((val & 0xFF00) >> 8);
    stack[stack_pointer + 1] = (unsigned char)((val & 0x00FF));
    stack_pointer += 2;
}

bool Chip8::wait_for_key() {
    if (key_register != (unsigned char)-1) {
        for (unsigned char i = 0x0; i <= 0xF; i++) {
            if (sf::Keyboard::isKeyPressed(CHIP_8_KEYS.at(i))) {
                registers[key_register] = i;
                key_register = -1;
            }
        }
    }
    return key_register != (unsigned char)-1;
}
