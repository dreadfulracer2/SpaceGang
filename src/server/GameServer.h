
#pragma once

#include <SFML/Network.hpp>
#include <list>
#include <cstdint>

#include "network/ClientSession.h"

class GameServer
{
private:
    static constexpr unsigned short PORT = 53000;
    static constexpr std::size_t MAX_PLAYERS = 4;

    sf::TcpListener listener;
    sf::SocketSelector selector;

    std::list<ClientSession> clients;

    std::uint32_t nextPlayerId = 1;
    bool running = false;

public:
    bool start();
    void run();
    void stop();

private:
    void acceptClient();
    void checkClients();
    void broadcastPlayerJoined(std::uint32_t playerId);
};
