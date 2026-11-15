
#include "GameServer.h"
#include "protocol/MessageType.h"

#include <iostream>
#include <vector>

bool GameServer::start()
{
    if (listener.listen(PORT) != sf::Socket::Status::Done)
        return false;

    listener.setBlocking(false);
    selector.add(listener);

    running = true;

    std::cout << "SpaceGang Server iniciado\n";
    return true;
}

void GameServer::run()
{
    while (running)
    {
        // Esperamos brevemente por conexiones o datos.
        selector.wait(sf::milliseconds(10));

        if (selector.isReady(listener))
            acceptClient();

        checkClients();
    }
}

void GameServer::acceptClient()
{
    clients.emplace_back(0);
    auto& session = clients.back();

    auto& socket = session.connection.getSocket();

    if (listener.accept(socket) != sf::Socket::Status::Done)
    {
        clients.pop_back();
        return;
    }

    // Una conexion nueva no recibe un espacio
    // hasta comprobar que existe disponibilidad.
    if (lobby.isFull())
    {
        session.connection.setConnected(true);

        Protocol::Message rejected = {
            static_cast<std::uint8_t>(
                MessageType::ServerFull
            )
        };

        session.connection.queueMessage(rejected);
        session.connection.flushSendQueue();
        session.connection.disconnect();

        clients.pop_back();

        std::cout << "Lobby lleno\n";
        return;
    }

    auto id = lobby.addPlayer("Jugador");

    if (!id)
    {
        session.connection.disconnect();
        clients.pop_back();
        return;
    }

    session.playerId = *id;
    session.connection.setConnected(true);

    Protocol::Message accepted = {
        static_cast<std::uint8_t>(
            MessageType::JoinAccepted
        ),
        static_cast<std::uint8_t>(session.playerId)
    };

    session.connection.queueMessage(accepted);

    if (!session.connection.flushSendQueue())
    {
        lobby.removePlayer(session.playerId);
        clients.pop_back();
        return;
    }

    selector.add(socket);

    sendLobbyState(session);
    broadcastPlayerJoined(session.playerId);

    std::cout << "Jugador "
        << session.playerId
        << " conectado\n";
}

void GameServer::checkClients()
{
    for (auto it = clients.begin(); it != clients.end();)
    {
        auto& connection = it->connection;

        if (selector.isReady(connection.getSocket()))
        {
            std::vector<Protocol::Message> messages;

            if (!connection.receiveMessages(messages))
            {
                disconnectClient(it);
                continue;
            }


            for (const auto& message : messages)
            {
                processMessage(*it, message);
            }

        }

        if (!connection.flushSendQueue())
        {
            disconnectClient(it);
            continue;
        }

        ++it;
    }
}

void GameServer::disconnectClient(
    std::list<ClientSession>::iterator& it)
{
    auto id = it->playerId;

    selector.remove(it->connection.getSocket());
    it->connection.disconnect();

    lobby.removePlayer(id);

    it = clients.erase(it);
    broadcastPlayerLeft(id);
    std::cout << "Jugador " << id
        << " desconectado\n";
}

void GameServer::stop()
{
    running = false;

    selector.clear();

    for (auto& client : clients)
        client.connection.disconnect();

    clients.clear();
    listener.close();
}

void GameServer::sendLobbyState(ClientSession& client)
{
    auto players = lobby.getPlayers();

    Protocol::Message message;

    message.push_back(
        static_cast<std::uint8_t>(
            MessageType::LobbyState
            )
    );

    message.push_back(
        static_cast<std::uint8_t>(players.size())
    );

    for (const auto& player : players)
    {
        message.push_back(
            static_cast<std::uint8_t>(player.playerId)
        );

        message.push_back(player.zoneId);
    }

    client.connection.queueMessage(message);
}

void GameServer::broadcastPlayerJoined(
    std::uint32_t playerId)
{
    Protocol::Message message = {
        static_cast<std::uint8_t>(
            MessageType::PlayerJoined
        ),
        static_cast<std::uint8_t>(playerId)
    };

    for (auto& client : clients)
    {
        if (client.playerId != playerId)
            client.connection.queueMessage(message);
    }
}

void GameServer::broadcastPlayerLeft(
    std::uint32_t playerId)
{
    Protocol::Message message = {
        static_cast<std::uint8_t>(
            MessageType::PlayerLeft
        ),
        static_cast<std::uint8_t>(playerId)
    };

    for (auto& client : clients)
        client.connection.queueMessage(message);
}

void GameServer::processMessage(
    ClientSession& client,
    const Protocol::Message& message)
{
    if (message.empty())
        return;

    auto type = static_cast<MessageType>(message[0]);

    switch (type)
    {
    case MessageType::SelectZone:
    {
        if (message.size() != 2)
            return;

        std::uint8_t zoneId = message[1];

        if (!lobby.setPlayerZone(
            client.playerId, zoneId))
        {
            std::cout << "Zona invalida solicitada por "
                << client.playerId << "\n";
            return;
        }

        std::cout << "Jugador "
            << client.playerId
            << " cambio a zona "
            << static_cast<int>(zoneId)
            << "\n";

        broadcastZoneChanged(
            client.playerId, zoneId
        );

        break;
    }

    default:
        break;
    }
}

void GameServer::broadcastZoneChanged(
    std::uint32_t playerId,
    std::uint8_t zoneId)
{
    Protocol::Message message = {
        static_cast<std::uint8_t>(
            MessageType::PlayerZoneChanged
        ),
        static_cast<std::uint8_t>(playerId),
        zoneId
    };

    for (auto& client : clients)
        client.connection.queueMessage(message);
}
