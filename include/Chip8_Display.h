#include "Chip8.h"
#include <SFML/Config.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>

class Chip8_Display {
    private:
        Chip8 chip8;
        sf::RenderWindow* window;
        int pixel_box_size;

        int width;
        int height;

        sf::Texture* texture;
        sf::Uint8* pixels;

    public:
        Chip8_Display(Chip8, int);
        void render(bool[]);
        ~Chip8_Display();

        inline sf::RenderWindow* get_window() { return window; }

    private:
        void initialize_window();
};
