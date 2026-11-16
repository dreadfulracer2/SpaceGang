
#include "NetworkClient.h"

#include "protocol/MessageType.h"

#include <SFML/Network.hpp>

#include <iostream>
#include <vector>

bool NetworkClient::connectToServer(
    const std::string& ip)
{
    auto address = sf::IpAddress::resolve(ip);

    if (!address)
        return false;

    auto& socket = connection.getSocket();

    if (socket.connect(
        *address,
        53000,
        sf::seconds(5)
    ) != sf::Socket::Status::Done)
    {
        return false;
    }

    connection.setConnected(true);

    std::cout << "Conectado al servidor\n";

    return true;
}

bool NetworkClient::update()
{
    if (!connection.isConnected())
        return false;

    if (!connection.flushSendQueue())
    {
        std::cout << "Error al enviar datos\n";
        disconnect();
        return false;
    }

    std::vector<Protocol::Message> messages;

    if (!connection.receiveMessages(messages))
    {
        std::cout << "Conexion finalizada\n";
        disconnect();
        return false;
    }

    for (const auto& message : messages)
    {
        processMessage(message);

        if (!connection.isConnected())
            return false;
    }

    return true;
}

void NetworkClient::processMessage(
    const Protocol::Message& message)
{
    if (message.empty())
        return;

    auto type = static_cast<MessageType>(message[0]);

    switch (type)
    {
    case MessageType::JoinAccepted:
    {
        if (message.size() != 2)
            break;

        playerId = message[1];

        std::cout
            << "Tu ID es: "
            << playerId << "\n";

        break;
    }

    case MessageType::LobbyState:
    {
        if (message.size() < 2)
            break;

        std::uint8_t count = message[1];

        if (count > 4 ||
            message.size() != 2 + count * 4)
            break;

        std::vector<PlayerInfo> players;

        for (std::size_t i = 0;
            i < count;
            ++i)
        {
            std::size_t offset = 2 + i * 4;

            PlayerInfo player;

            player.playerId = message[offset];
            player.zoneId = message[offset + 1];
            player.secondaryZoneId = message[offset + 2];
            player.thirdZoneId = message[offset + 3];

            player.name =
                "Jugador " +
                std::to_string(player.playerId);

            player.isHost = (player.playerId == 1);

            players.push_back(player);
        }

        lobby.setPlayers(players);
        lobby.printPlayers();

        break;
    }

    case MessageType::PlayerJoined:
    {
        if (message.size() != 2)
            break;

        lobby.addPlayer(message[1]);
        lobby.printPlayers();

        break;
    }

    case MessageType::PlayerLeft:
    {
        if (message.size() != 2)
            break;

        lobby.removePlayer(message[1]);
        lobby.printPlayers();

        break;
    }

    case MessageType::PlayerZoneChanged:
    {
        if (message.size() != 5)
            break;

        std::uint32_t id = message[1];
        std::uint8_t zone = message[2];
        std::uint8_t secondary = message[3];
        std::uint8_t third = message[4];

        lobby.updateZone(
            id,
            zone,
            secondary,
            third
        );

        lobby.printPlayers();

        break;
    }

    case MessageType::ServerFull:
    {
        std::cout << "Servidor lleno\n";
        disconnect();
        break;
    }

    case MessageType::GameStarted:
    {
        if (message.size() != 1)
            break;

        gameState = GameState::Playing;

        rocketVotePending = false;
        rocketVoteConfirmed = false;
        rocketVotingFinished = false;

        rocketVotesReceived = 0;
        rocketTotalPlayers = 0;

        rocketWinners = { 0, 0, 0 };

        std::cout << "La partida ha comenzado\n";

        break;
    }

    case MessageType::StartGameRejected:
    {
        if (message.size() != 1)
            break;

        startGameRejected = true;

        std::cout
            << "No se pudo iniciar la partida\n";

        break;
    }


    case MessageType::RocketVoteAccepted:
    {
        if (message.size() != 1)
            break;

        rocketVotePending = false;
        rocketVoteConfirmed = true;

        std::cout << "Votos confirmados.\n";
        break;
    }

    case MessageType::RocketVoteRejected:
    {
        if (message.size() != 1)
            break;

        rocketVotePending = false;

        std::cout << "El servidor rechazo los votos.\n";
        break;
    }

    case MessageType::RocketVotingProgress:
    {
        if (message.size() != 3)
            break;

        rocketVotesReceived = message[1];
        rocketTotalPlayers = message[2];

        std::cout
            << "Progreso: "
            << static_cast<int>(rocketVotesReceived)
            << " / "
            << static_cast<int>(rocketTotalPlayers)
            << "\n";

        break;
    }

    case MessageType::RocketVotingFinished:
    {
        if (message.size() != 4)
            break;

        rocketVotePending = false;
        rocketVotingFinished = true;

        rocketWinners = {
            message[1],
            message[2],
            message[3]
        };

        std::cout << "VOTING COMPLETE.\n";
        break;
    }


    default:
        break;
    }
}

bool NetworkClient::selectZone(
    std::uint8_t zoneId)
{
    if (!connection.isConnected())
        return false;

    if (zoneId < 1 || zoneId > 3)
        return false;

    Protocol::Message msg = {
        static_cast<std::uint8_t>(
            MessageType::SelectZone
        ),
        zoneId
    };

    return connection.queueMessage(msg);
}

void NetworkClient::disconnect()
{
    connection.disconnect();

    lobby.clear();

    playerId = 0;
    gameState = GameState::Lobby;
    startGameRejected = false;

    rocketVotePending = false;
    rocketVoteConfirmed = false;
    rocketVotingFinished = false;

    rocketVotesReceived = 0;
    rocketTotalPlayers = 0;

    rocketWinners = { 0, 0, 0 };

}

std::uint32_t NetworkClient::getPlayerId() const
{
    return playerId;
}

const ClientLobby& NetworkClient::getLobby() const
{
    return lobby;
}

bool NetworkClient::requestStartGame()
{
    if (!connection.isConnected())
        return false;

    if (gameState != GameState::Lobby)
        return false;

    Protocol::Message msg = {
        static_cast<std::uint8_t>(
            MessageType::StartGameRequest
        )
    };

    return connection.queueMessage(msg);
}

GameState NetworkClient::getGameState() const
{
    return gameState;
}

bool NetworkClient::consumeStartGameRejected()
{
    bool rejected = startGameRejected;
    startGameRejected = false;
    return rejected;
}


bool NetworkClient::submitRocketVote(
    std::uint8_t crewModule,
    std::uint8_t booster1,
    std::uint8_t booster2)
{
    if (!connection.isConnected())
        return false;

    if (gameState != GameState::Playing)
        return false;

    if (rocketVoteConfirmed ||
        rocketVotePending ||
        rocketVotingFinished)
    {
        return false;
    }

    if (crewModule > 2 ||
        booster1 > 2 ||
        booster2 > 2)
    {
        return false;
    }

    Protocol::Message message = {
        static_cast<std::uint8_t>(
            MessageType::SubmitRocketVote
        ),
        crewModule,
        booster1,
        booster2
    };

    if (!connection.queueMessage(message))
        return false;

    // Bloquear envios duplicados mientras
    // esperamos la respuesta del servidor.
    rocketVotePending = true;

    return true;
}


bool NetworkClient::hasConfirmedRocketVote() const
{
    return rocketVoteConfirmed;
}

bool NetworkClient::isRocketVotePending() const
{
    return rocketVotePending;
}

bool NetworkClient::isRocketVotingFinished() const
{
    return rocketVotingFinished;
}

std::uint8_t NetworkClient::getRocketVotesReceived() const
{
    return rocketVotesReceived;
}

std::uint8_t NetworkClient::getRocketTotalPlayers() const
{
    return rocketTotalPlayers;
}

const std::array<std::uint8_t, 3>&
NetworkClient::getRocketWinners() const
{
    return rocketWinners;
}
