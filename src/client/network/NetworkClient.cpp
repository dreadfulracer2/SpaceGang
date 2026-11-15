
#include "NetworkClient.h"
#include "protocol/MessageType.h"

#include <iostream>

bool NetworkClient::connectToServer(
    const std::string& ip)
{
    auto address = sf::IpAddress::resolve(ip);

    if (!address)
        return false;

    if (socket.connect(*address, 53000,
        sf::seconds(5)) != sf::Socket::Status::Done)
    {
        return false;
    }

    std::cout << "Conectado al servidor\n";
    return true;
}

void NetworkClient::receiveMessages()
{
    while (true)
    {
        sf::Packet packet;

        auto status = socket.receive(packet);

        if (status != sf::Socket::Status::Done)
        {
            std::cout << "Conexion finalizada\n";
            break;
        }

        std::uint8_t type;
        if (!(packet >> type))
            continue;

        switch (static_cast<MessageType>(type))
        {
        case MessageType::JoinAccepted:
        {
            if (!(packet >> playerId))
                break;

            std::cout << "Tu ID es: "
                << playerId << "\n";
            break;
        }

        case MessageType::PlayerJoined:
        {
            std::uint32_t id;

            if (!(packet >> id))
                break;

            std::cout << "Jugador "
                << id << " conectado\n";
            break;
        }

        case MessageType::ServerFull:
        {
            std::cout << "Servidor lleno\n";
            return;
        }

        default:
            break;
        }
    }
}

void NetworkClient::disconnect()
{
    socket.disconnect();
}

std::uint32_t NetworkClient::getPlayerId() const
{
    return playerId;
}
