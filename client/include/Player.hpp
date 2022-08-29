#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include <SFML/Graphics.hpp>

class Player : public sf::Drawable
{
    public:
        Player(const sf::Color &color);
        void setPosition(const sf::Vector2f &position);
    private:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
        sf::CircleShape shape;
};

#endif