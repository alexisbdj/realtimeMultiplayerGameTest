#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <algorithm>
#include <cmath>
#include "Server.hpp"

Server::Server()
{
    this->idCount = 0;
    this->socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->socketFd == -1) {
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
        throw std::runtime_error("failed to init server");
    }

    sockaddr_in addr{};
    
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);


    if (bind(this->socketFd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        switch (errno) {
            case EACCES:
                std::cout << "access denied" << std::endl;
                break;
            case EADDRINUSE:
                std::cout << "address in use" << std::endl;
                break;
            default:
                std::cout << "unknown bind error" << std::endl;
        }
        throw std::runtime_error("failed to init server");
    }
}

Server::~Server()
{
    close(this->socketFd);
}

int Server::run()
{
    if (listen(this->socketFd, 1) == -1) {
        switch (errno) {
            case ENOTSOCK:
                std::cout << "not a socket" << std::endl;
                break;
            case EOPNOTSUPP:
                std::cout << "socket no support" << std::endl;
                break;
            case EADDRINUSE:
                std::cout << "address in use" << std::endl;
                break;
            case EBADF:
                std::cout << "bad file descriptor" << std::endl;
                break;
            default:
                std::cout << "rip" << std::endl;
                break;
        }
        return -1;
    }
    this->startClientThread();

    this->mainLoop();
    return 0;
}

void Server::startClientThread()
{
    this->clientThreads.push_back(std::thread(&Server::acceptClients, this));
}

void Server::mainLoop()
{
    std::chrono::high_resolution_clock clock;
    std::chrono::microseconds timePerFrames = std::chrono::microseconds(16666);
    while (true) {
        auto b = clock.now();
        this->frame();
        std::this_thread::sleep_until(b + timePerFrames);
    }
}

void Server::frame()
{
    const float speed = 5;
    const float deltaTime = 1.f/60.f;
    for (auto &it : this->players) {
        Player &player = it.second;
        float vx = 0;
        float vy = 0;

        if (player.moving & MOVING_LEFT_BIT) {
            vx -= 1.f;
        }
        if (player.moving & MOVING_RIGHT_BIT) {
            vx += 1.f;
        }
        if (player.moving & MOVING_TOP_BIT) {
            vy -= 1.f;
        }
        if (player.moving & MOVING_BOTTOM_BIT) {
            vy += 1.f;
        }
        if (vx == 0 && vy == 0)
            return;

        float norm = sqrt((vx * vx) + (vy * vy));
        vx = (vx / norm) * deltaTime;
        vy = (vy / norm) * deltaTime;
        player.posx += vx;
        player.posy += vy;
    }
}

void Server::sendGameState(int connfd)
{
    try {
        Player &player = this->players.at(connfd);
        std::set<int> unknownPlayers;
        for (auto &it : this->players)
        {
            if (player.knownPlayers.find(it.second.id) == player.knownPlayers.end()) {
                unknownPlayers.insert(it.second.id);
            }
        }

        size_t size = unknownPlayers.size();
        write(connfd, &size, sizeof(size_t));
        for (auto &it : unknownPlayers) {
            auto &p = this->getPlayerById(it);
            PlayerAddPacket packet = {p.id, p.color};
            write(connfd, &packet, sizeof(PlayerAddPacket));
            player.knownPlayers.insert(it);
        }
        
        size = this->players.size();

        write(connfd, &size, sizeof(size_t));
        for (auto &it : this->players) {
            PlayerStatePacket packet = {it.second.id, it.second.posx, it.second.posy};
            write(connfd, &packet, sizeof(PlayerStatePacket));
        }

        std::set<int> lostPlayers;

        for (auto &it : player.knownPlayers) {
            try {
                this->getPlayerById(it);
            }
            catch (std::out_of_range &err) {
                lostPlayers.insert(it);
            }
        }

        
        size = lostPlayers.size();

        write(connfd, &size, sizeof(size_t));
        for (auto &it : lostPlayers) {
            write(connfd, &it, sizeof(int));
        }
    }
    catch (std::out_of_range &err)
    {
        std::cerr << "socket with unknown player" << std::endl;
    }
}

void Server::acceptClients()
{
    int connfd;
    sockaddr_in cliaddr;
    socklen_t cliaddrlen = sizeof(cliaddr);

    connfd = accept(this->socketFd, (struct sockaddr *) &cliaddr, &cliaddrlen);
    if (connfd == -1) {
        std::cout << "erreur c'est terrible" << std::endl;
        return;
    }

    this->startClientThread();

    this->addPlayer(connfd);

    Player &player = this->players.at(connfd);

    while (read(connfd, &player.moving, sizeof(uint8_t))) {
        sendGameState(connfd);
    }

    std::cout << this->players.at(connfd).id << " disconnected" << std::endl;

    this->players.erase(connfd);
}

void Server::addPlayer(int connfd)
{
    std::cout << "player " << this->idCount << " joined" << std::endl;
    this->players.insert({connfd, (Player) {this->idCount++, rand() % 4, (float) (rand() % 10), (float) (rand() % 8), 0, std::set<int>()}});
}

Player &Server::getPlayerById(int id)
{
    for (auto &it : this->players) {
        if (it.second.id == id)
            return it.second;
    }
    throw std::out_of_range("no id found");
}