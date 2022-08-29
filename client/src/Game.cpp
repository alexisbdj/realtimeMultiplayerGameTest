#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Game.hpp"

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
    }
}

void Game::update()
{
    write(this->socketfd, "a", 1);
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
}

void Game::render()
{
    this->window.clear();
    this->window.setView(this->world.getView());
    this->window.draw(this->world);
    this->window.display();
}