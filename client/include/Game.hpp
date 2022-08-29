#ifndef GAME_HPP_
#define GAME_HPP_

#include <SFML/Graphics.hpp>
#include "World.hpp"

class Game
{
    public:
        Game(const char * serverAddr);
        ~Game();
        int run();
    private:
        void processEvent();
        void update();
        void render();

        void updateMovement(sf::Keyboard::Key key, bool value);
    
        sf::RenderWindow window;
        World world;

        int socketfd;
        uint8_t movement;
};

#endif