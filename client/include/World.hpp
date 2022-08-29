#ifndef WORLD_HPP_
#define WORLD_HPP_

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "Player.hpp"

class World : public sf::Drawable {
    public:
        World();

        sf::View &getView();

        void setPosition(int id, const sf::Vector2f& position);
        void addPlayer(int id, const sf::Color &color);
        void removePlayer(int id);
    private:
        sf::View view;
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

        std::unordered_map<int, Player> players;
};

#endif