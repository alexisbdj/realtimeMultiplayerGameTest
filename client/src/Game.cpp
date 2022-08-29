#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Game.hpp"

#define MOVING_LEFT_BIT 1
#define MOVING_RIGHT_BIT 2
#define MOVING_TOP_BIT 4
#define MOVING_BOTTOM_BIT 8

struct PlayerAddPacket
{
    int id;
    int color;
};

struct PlayerStatePacket
{
    int id;
    float posx;
    float posy;
};

Game::Game(const char * serverAddr) : window(sf::VideoMode(1000, 800), "mmo de fou")
{
    this->socketfd = socket(AF_INET, SOCK_STREAM, 0);
    this->movement = 0;
    if (this->socketfd == -1) {
        switch (errno) {
            case EPROTONOSUPPORT:
                std::cout << "no protocol support" << std::endl;
                break;
            case EACCES:
                std::cout << "access denied" << std::endl;
                break;
            default:
                std::cout << "??? error" << std::endl;
        }
        throw std::runtime_error("a");
    }

    sockaddr_in addr{};

    inet_pton(AF_INET, serverAddr, &addr.sin_addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);

    if (connect(this->socketfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        std::cout << "can't connect" << std::endl;
        throw std::runtime_error("a");
    }
}

Game::~Game()
{
    close(this->socketfd);
}

int Game::run()
{
    sf::Clock clock;
    sf::Time targetTime = sf::seconds(1.f / 60.f);

    while (this->window.isOpen()) {
        this->processEvent();
        this->update();
        this->render();

        sf::sleep(targetTime - clock.restart());
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
        else if (event.type == sf::Event::KeyPressed) {
            updateMovement(event.key.code, true);
        }
        else if (event.type == sf::Event::KeyReleased) {
            updateMovement(event.key.code, false);
        }
    }
}

void Game::updateMovement(sf::Keyboard::Key key, bool value)
{
    uint8_t bit = 0;
    
    if (key == sf::Keyboard::Q) {
        bit = MOVING_LEFT_BIT;
    }
    else if (key == sf::Keyboard::D) {
        bit = MOVING_RIGHT_BIT;
    }
    else if (key == sf::Keyboard::Z) {
        bit = MOVING_TOP_BIT;
    }
    else if (key == sf::Keyboard::S) {
        bit = MOVING_BOTTOM_BIT;
    }
    else {
        return;
    }
    if (value) {
        this->movement |= bit;
    }
    else {
        this->movement &= (~bit);
    }
}

void Game::update()
{
    std::cout << (this->movement & 1) << (this->movement & 2) << (this->movement & 4) << (this->movement & 8) << std::endl;
    write(this->socketfd, &this->movement, sizeof(uint8_t));

    const sf::Color colors[] = {
        sf::Color::Red,
        sf::Color::Cyan,
        sf::Color::Green,
        sf::Color::Yellow,
    };

    size_t nNewPlayers;
    read(this->socketfd, &nNewPlayers, sizeof(size_t));
    for (size_t i = 0; i < nNewPlayers; i++) {
        PlayerAddPacket addPacket;
        read(this->socketfd, &addPacket, sizeof(addPacket));
        this->world.addPlayer(addPacket.id, colors[addPacket.color]);
    }


    size_t nPlayer;
    read(this->socketfd, &nPlayer, sizeof(size_t));
    
    for (size_t i = 0; i < nPlayer; i++) {

        PlayerStatePacket statePacket;
        read(this->socketfd, &statePacket, sizeof(statePacket));
        this->world.setPosition(statePacket.id, sf::Vector2f(statePacket.posx, statePacket.posy));
    }

    size_t lostPlayers;
    read(this->socketfd, &lostPlayers, sizeof(size_t));

    for (size_t i = 0; i < lostPlayers; i++) {
        int id;
        read(this->socketfd, &id, sizeof(int));
        this->world.removePlayer(id);
    }
}

void Game::render()
{
    this->window.clear();
    this->window.setView(this->world.getView());
    this->window.draw(this->world);
    this->window.display();
}