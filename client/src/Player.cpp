#include "Player.hpp"

Player::Player(const sf::Color &color)
{
    this->shape.setRadius(0.5);
    this->shape.setFillColor(color);
    this->shape.setPosition(0, 0);
    this->shape.setOrigin(0.5, 0.5);
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(this->shape, states);
}

void Player::setPosition(const sf::Vector2f &position)
{
    this->shape.setPosition(position);
}