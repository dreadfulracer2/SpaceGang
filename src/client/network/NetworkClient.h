
#pragma once

#include "network/TcpConnection.h"
#include "lobby/ClientLobby.h"
#include "game/GameState.h"
#include <cstdint>
#include <string>

class NetworkClient
{
private:
    TcpConnection connection;
    ClientLobby lobby;

    std::uint32_t playerId = 0;

    void processMessage(
        const Protocol::Message& message
    );
    GameState gameState = GameState::Lobby;
    bool startGameRejected = false;

public:
    bool connectToServer(const std::string& ip);

    bool update();

    void disconnect();

    std::uint32_t getPlayerId() const;

    const ClientLobby& getLobby() const;

    bool selectZone(std::uint8_t zoneId);
    bool requestStartGame();

    GameState getGameState() const;

    bool consumeStartGameRejected();
};
