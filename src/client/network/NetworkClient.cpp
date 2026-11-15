
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

void NetworkClient::receiveMessages()
{
    while (connection.isConnected())
    {

        if (!connection.flushSendQueue())
        {
            std::cout << "Error al enviar datos\n";
            break;
        }

        std::vector<Protocol::Message> messages;

        if (!connection.receiveMessages(messages))
        {
            std::cout << "Conexion finalizada\n";
            break;
        }

        for (const auto& message : messages)
        {
            if (message.empty())
                continue;

            auto type = static_cast<MessageType>(
                message[0]
                );

            switch (type)
            {
            case MessageType::JoinAccepted:
            {
                if (message.size() != 2)
                    break;

                playerId = message[1];

                std::cout << "Tu ID es: "
                    << playerId << "\n";

                std::uint8_t zoneId =
                    static_cast<std::uint8_t>(
                        ((playerId - 1) % 3) + 1
                        );

                selectZone(zoneId);

                break;
            }

            case MessageType::LobbyState:
            {
                if (message.size() < 2)
                    break;

                std::uint8_t count = message[1];

                if (count > 4 ||
                    message.size() != 2 + count * 2)
                {
                    break;
                }

                std::vector<PlayerInfo> players;

                for (std::size_t i = 0; i < count; ++i)
                {
                    std::size_t offset = 2 + i * 2;

                    PlayerInfo player;
                    player.playerId = message[offset];
                    player.zoneId = message[offset + 1];

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

                auto id = message[1];

                lobby.addPlayer(id);

                std::cout << "Jugador "
                    << static_cast<int>(id)
                    << " entro al lobby\n";

                lobby.printPlayers();

                break;
            }

            case MessageType::PlayerLeft:
            {
                if (message.size() != 2)
                    break;

                auto id = message[1];

                lobby.removePlayer(id);

                std::cout << "Jugador "
                    << static_cast<int>(id)
                    << " salio del lobby\n";

                lobby.printPlayers();

                break;
            }

            case MessageType::ServerFull:
            {
                std::cout << "Servidor lleno\n";
                disconnect();
                return;
            }

            case MessageType::PlayerZoneChanged:
            {
                if (message.size() != 3)
                    break;

                std::uint32_t id = message[1];
                std::uint8_t zoneId = message[2];

                lobby.updateZone(id, zoneId);

                std::cout << "Jugador " << id
                    << " cambio a Zona "
                    << static_cast<int>(zoneId)
                    << "\n";

                lobby.printPlayers();

                break;
            }

            default:
                break;
            }
        }

        sf::sleep(sf::milliseconds(10));
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
