
#pragma once

#include "network/TcpConnection.h"
#include "lobby/ClientLobby.h"
#include "game/GameState.h"
#include <cstdint>
#include <string>
#include <array>
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

    bool rocketVotePending = false;
    bool rocketVoteConfirmed = false;
    bool rocketVotingFinished = false;

    std::uint8_t rocketVotesReceived = 0;
    std::uint8_t rocketTotalPlayers = 0;

    std::array<std::uint8_t, 3> rocketWinners = {
        0, 0, 0
    };


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
    bool submitRocketVote(
        std::uint8_t crewModule,
        std::uint8_t booster1,
        std::uint8_t booster2
    );

    bool hasConfirmedRocketVote() const;

    bool isRocketVotePending() const;

    bool isRocketVotingFinished() const;

    std::uint8_t getRocketVotesReceived() const;

    std::uint8_t getRocketTotalPlayers() const;

    const std::array<std::uint8_t, 3>&
        getRocketWinners() const;

};
