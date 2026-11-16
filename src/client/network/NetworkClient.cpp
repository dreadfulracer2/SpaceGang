
#include "NetworkClient.h"
#include "protocol/MessageType.h"
#include <atomic>
#include <thread>
#include <mutex>
#include <iostream>
#include <vector>

bool NetworkClient::connectToServer(const std::string& ip)
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

        std::cout << "Tu ID es: "
            << playerId << "\n";

        // Eliminamos la seleccion automatica
        // de zona que usamos en la etapa anterior.
        break;
    }

    case MessageType::LobbyState:
    {
        if (message.size() < 2)
            break;

        const std::uint8_t count = message[1];

        if (count > 4 ||
            message.size() != 2 + count * 2)
            break;

        std::vector<PlayerInfo> players;

        for (std::size_t i = 0; i < count; ++i)
        {
            const std::size_t offset = 2 + i * 2;

            PlayerInfo player;
            player.playerId = message[offset];
            player.zoneId = message[offset + 1];
            player.name = "Jugador " +
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
        if (message.size() != 3)
            break;

        lobby.updateZone(message[1], message[2]);

        std::cout << "Jugador "
            << static_cast<int>(message[1])
            << " cambio a zona "
            << static_cast<int>(message[2])
            << "\n";

        lobby.printPlayers();
        break;
    }

    case MessageType::ServerFull:
    {
        std::cout << "Servidor lleno\n";
        disconnect();
        break;
    }

    default:
        break;
    }
}

void NetworkClient::disconnect()
{
    connection.disconnect();
    lobby.clear();
    playerId = 0;
}

std::uint32_t NetworkClient::getPlayerId() const
{
    return playerId;
}

const ClientLobby& NetworkClient::getLobby() const
{
    return lobby;
}

bool NetworkClient::selectZone(std::uint8_t zoneId)
{
    if (!connection.isConnected())
        return false;

    if (zoneId < 1 || zoneId > 3)
        return false;

    Protocol::Message message = {
        static_cast<std::uint8_t>(
            MessageType::SelectZone
        ),
        zoneId
    };

    return connection.queueMessage(message);
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
