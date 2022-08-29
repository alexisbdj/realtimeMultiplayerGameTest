#include <iostream>
#include "World.hpp"

World::World()
{
    this->view.reset(sf::FloatRect(0, 0, 10, 8));
}

sf::View &World::getView()
{
    return this->view;
}

void World::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (auto &it : this->players) {
        target.draw(it.second, states);
    }
}

void World::setPosition(int id, const sf::Vector2f& position)
{
    try {
        this->players.at(id).setPosition(position);
    }
    catch (std::out_of_range &e)
    {
        std::cerr << "moving unknown player" << std::endl;
    }
}

void World::addPlayer(int id, const sf::Color &color)
{
    this->players.insert({id, Player(color)});
}

void World::removePlayer(int id)
{
    this->players.erase(id);
}