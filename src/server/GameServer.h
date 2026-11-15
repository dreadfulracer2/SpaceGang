
#pragma once

#include <SFML/Network.hpp>

#include <cstdint>
#include <list>

#include "network/ClientSession.h"
#include "lobby/LobbyManager.h"

class GameServer
{
private:
    static constexpr unsigned short PORT = 53000;

    sf::TcpListener listener;
    sf::SocketSelector selector;

    std::list<ClientSession> clients;
    LobbyManager lobby;

    bool running = false;

public:
    bool start();
    void run();
    void stop();

private:
    void acceptClient();
    void checkClients();

    void disconnectClient(
        std::list<ClientSession>::iterator& it
    );

    void sendLobbyState(ClientSession& client);

    void broadcastPlayerJoined(
        std::uint32_t playerId
    );

    void broadcastPlayerLeft(
        std::uint32_t playerId
    );

    void processMessage(
        ClientSession& client,
        const Protocol::Message& message
    );

    void broadcastZoneChanged(
        std::uint32_t playerId,
        std::uint8_t zoneId
    );

};
