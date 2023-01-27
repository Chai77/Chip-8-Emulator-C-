#include <SFML/Graphics/RenderWindow.hpp>
#include <chrono>
#include <iostream>
#include "Chip8.h"
#include "Chip8_Display.h"
#include "constants.h"

#include <SFML/Graphics.hpp>

int main()
{
    Chip8 chip8;

    chip8.load_rom_to_memory("roms/Space Invaders.ch8");
    // chip8.load_rom_to_memory("roms/known_test.ch8");
    // chip8.load_rom_to_memory("roms/test_opcode.ch8");
    // chip8.load_rom_to_memory("roms/c8_test.c8");

    auto lastInstructionTime = std::chrono::high_resolution_clock::now();
    auto lastTimerUpdateTime = std::chrono::high_resolution_clock::now();

    float timer_delay = 1000.0f/TIMER_HZ;
    float instruction_delay = 1000.0f/INSTRUCTION_HZ;

    Chip8_Display display(chip8, 10);
    sf::RenderWindow* window = display.get_window();
    while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window->close();
        }

        if (chip8.draw_flag) {
            display.render(chip8.frame_buffer);
            chip8.draw_flag = false;
        }
        auto currentTime = std::chrono::high_resolution_clock::now();

        float instructionDifference = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastInstructionTime).count();
        float timerUpdateDifference = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastTimerUpdateTime).count();

        if (instructionDifference > instruction_delay) {
            lastInstructionTime = currentTime;
            chip8.complete_one_instruction();
        }
        if (timerUpdateDifference > timer_delay) {
            lastTimerUpdateTime = currentTime;
            chip8.update_timers();
        }

        // chip8.complete_one_instruction();
        // chip8.update_timers();
    }
    return 0;
}
