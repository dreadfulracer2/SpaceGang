
#pragma once

#include <SFML/Network.hpp>

#include <cstdint>
#include <list>

#include "network/ClientSession.h"
#include "lobby/LobbyManager.h"
#include "game/GameState.h"
#include "voting/RocketVotingManager.h"
#include "simulation/RocketLaunchSimulator.h"
enum class RocketVotingState
{
    Inactive,
    Voting,
    Finished
};

class GameServer
{
private:
    static constexpr unsigned short PORT = 53000;

    sf::TcpListener listener;
    sf::SocketSelector selector;

    std::list<ClientSession> clients;

    LobbyManager lobby;

    bool running = false;

    // Estado actual de la partida.
    GameState gameState = GameState::Lobby;
    RocketLaunchSimulator launchSimulator;

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

    void broadcastAllZones();

    void processMessage(
        ClientSession& client,
        const Protocol::Message& message
    );

    // Nuevas funciones de la etapa 6.4.
    void broadcastGameStarted();

    void sendStartGameRejected(
        ClientSession& client
    );
    RocketVotingManager rocketVoting;
    void broadcastRocketVotingProgress();
    void broadcastRocketVotingFinished();
    RocketVotingState votingState =
        RocketVotingState::Inactive;
};
