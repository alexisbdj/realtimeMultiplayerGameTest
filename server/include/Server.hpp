#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <vector>
#include <thread>
#include <unordered_map>
#include <set>

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

struct Player
{
    int id;
    int color;
    float posx;
    float posy;
    uint8_t moving;
    std::set<int> knownPlayers;
};

class Server
{
    public:
        Server();
        ~Server();

        int run();
    private:
        void frame();
        void mainLoop();
        void acceptClients();
        void addPlayer(int connfd);
        Player &getPlayerById(int id);

        void startClientThread();

        void sendGameState(int connfd, uint8_t movement);

        int socketFd;
        std::vector<std::thread> clientThreads;
        std::unordered_map<int, Player> players;
        int idCount;
};

#endif