#ifndef GAME_HPP_
#define GAME_HPP_

#include <SFML/Graphics.hpp>
#include "World.hpp"

class Game
{
    public:
        Game(const char * serverAddr);
        int run();
    private:
        void processEvent();
        void update();
        void render();
    
        sf::RenderWindow window;
        World world;
};

#endif