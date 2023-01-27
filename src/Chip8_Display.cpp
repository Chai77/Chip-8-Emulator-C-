#include "Chip8_Display.h"
#include "constants.h"
#include <SFML/Config.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>

Chip8_Display::Chip8_Display(Chip8 chip8, int pixel_box_size) {
    this->chip8 = chip8;
    this->pixel_box_size = pixel_box_size;
    this->texture = new sf::Texture();

    initialize_window();

    texture->create(width, height);
    texture->setSmooth(false);
    texture->setRepeated(false);
    pixels = new sf::Uint8[height * (width * 4)];
}

void Chip8_Display::initialize_window() {
    width = pixel_box_size * PIXELS_WIDTH;
    height = pixel_box_size * PIXELS_HEIGHT;
    window = new sf::RenderWindow(sf::VideoMode(width, height), "Chip 8 Emulator");
}

void Chip8_Display::render(bool frame_buffer[]) {
    sf::RectangleShape rect;

    // bool* frame_buffer = chip8.get_frame_buffer();
    // bool* frame_buffer = chip8.frame_buffer;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        printf("\n\n\nThis is the frame buffer of the display one: \n");
        for (int i = 0; i < PIXELS_HEIGHT; i++) {
            for (int k = 0; k < PIXELS_WIDTH; k++) {
                printf("%d ", chip8.frame_buffer[i * PIXELS_WIDTH + k]);
            }
            printf("\n");
        }
        printf("\n\n\n\n");
    }

    for (int i = 0; i < PIXELS_HEIGHT; i++) {
        for (int k = 0; k < PIXELS_WIDTH; k++) {
            bool curr_pixel_val = frame_buffer[i * PIXELS_WIDTH + k];
            for (int r = 0; r < pixel_box_size; r++) {
                for (int c = 0; c < pixel_box_size; c++) {
                    int i_pixels = i * pixel_box_size + r;
                    int k_pixels = (k * pixel_box_size + c) * 4;
                    if (!curr_pixel_val) {
                        pixels[i_pixels * (width * 4) + k_pixels] = 255;
                        pixels[i_pixels * (width * 4) + k_pixels + 1] = 255;
                        pixels[i_pixels * (width * 4) + k_pixels + 2] = 255;
                        pixels[i_pixels * (width * 4) + k_pixels + 3] = 255;
                    } else {
                        pixels[i_pixels * (width * 4) + k_pixels] = 0;
                        pixels[i_pixels * (width * 4) + k_pixels + 1] = 0;
                        pixels[i_pixels * (width * 4) + k_pixels + 2] = 0;
                        pixels[i_pixels * (width * 4) + k_pixels + 3] = 255;
                    }
                }
            }
        }
    }
    texture->update(pixels);

    rect.setPosition(0, 0);
    rect.setSize({(float)width, (float)height});
    rect.setTexture(texture);

    window->clear();
    window->draw(rect);
    window->display();
}

Chip8_Display::~Chip8_Display() {
    delete window;
}
