#include <iostream>
#include "Game.hpp"

Game::Game(const char * serverAddr) : window(sf::VideoMode(1000, 800), "mmo de fou")
{
}

int Game::run()
{
    sf::Clock clock;
    sf::Time targetTime = sf::seconds(1.f / 60.f);
    int i = 0;

    while (this->window.isOpen()) {
        this->processEvent();
        this->update();
        this->render();

        sf::sleep(targetTime - clock.restart());
        std::cout << i++ << std::endl;
    }
    return 0;
}

void Game::processEvent()
{
    sf::Event event;
    while (this->window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }
}

void Game::update()
{

}

void Game::render()
{
    this->window.clear();
    this->window.setView(this->world.getView());
    this->window.draw(this->world);
    this->window.display();
}