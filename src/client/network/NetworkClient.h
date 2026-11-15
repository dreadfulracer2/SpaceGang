
#pragma once

#include "network/TcpConnection.h"
#include "lobby/ClientLobby.h"

#include <cstdint>
#include <string>

class NetworkClient
{
private:
    TcpConnection connection;
    ClientLobby lobby;

    std::uint32_t playerId = 0;

public:
    bool connectToServer(const std::string& ip);
    bool selectZone(std::uint8_t zoneId);
    void receiveMessages();
    void disconnect();

    std::uint32_t getPlayerId() const;

    const ClientLobby& getLobby() const;
};
void run();