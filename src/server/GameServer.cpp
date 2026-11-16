
#include "GameServer.h"

#include "protocol/MessageType.h"

#include <iostream>
#include <vector>

bool GameServer::start()
{
    if (listener.listen(PORT) !=
        sf::Socket::Status::Done)
    {
        std::cout << "Error al iniciar servidor\n";
        return false;
    }

    listener.setBlocking(false);
    selector.add(listener);

    running = true;

    std::cout << "SpaceGang Server iniciado\n";
    std::cout << "Puerto: " << PORT << "\n";

    return true;
}

void GameServer::run()
{
    while (running)
    {
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

    if (listener.accept(socket) !=
        sf::Socket::Status::Done)
    {
        clients.pop_back();
        return;
    }

    session.connection.setConnected(true);

    if (lobby.isFull() ||
        gameState != GameState::Lobby)
    {
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

    if (!id.has_value())
    {
        session.connection.disconnect();
        clients.pop_back();
        return;
    }

    session.playerId = *id;

    Protocol::Message accepted = {
        static_cast<std::uint8_t>(
            MessageType::JoinAccepted
        ),
        static_cast<std::uint8_t>(
            session.playerId
        )
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

    broadcastAllZones();

    std::cout << "Jugador "
        << session.playerId
        << " conectado\n";
}

void GameServer::checkClients()
{
    for (auto it = clients.begin();
        it != clients.end();)
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
    std::uint32_t id = it->playerId;

    selector.remove(it->connection.getSocket());

    it->connection.disconnect();

    lobby.removePlayer(id);

    it = clients.erase(it);

    broadcastPlayerLeft(id);

    broadcastAllZones();

    std::cout << "Jugador "
        << id
        << " desconectado\n";
}

void GameServer::sendLobbyState(
    ClientSession& client)
{
    auto players = lobby.getPlayers();

    Protocol::Message msg = {
        static_cast<std::uint8_t>(
            MessageType::LobbyState
        ),
        static_cast<std::uint8_t>(
            players.size()
        )
    };

    for (const auto& player : players)
    {
        msg.push_back(
            static_cast<std::uint8_t>(
                player.playerId
                )
        );

        msg.push_back(player.zoneId);
        msg.push_back(player.secondaryZoneId);
        msg.push_back(player.thirdZoneId);
    }

    client.connection.queueMessage(msg);
}

void GameServer::broadcastPlayerJoined(
    std::uint32_t playerId)
{
    Protocol::Message msg = {
        static_cast<std::uint8_t>(
            MessageType::PlayerJoined
        ),
        static_cast<std::uint8_t>(playerId)
    };

    for (auto& client : clients)
    {
        if (client.playerId != playerId)
            client.connection.queueMessage(msg);
    }
}

void GameServer::broadcastPlayerLeft(
    std::uint32_t playerId)
{
    Protocol::Message msg = {
        static_cast<std::uint8_t>(
            MessageType::PlayerLeft
        ),
        static_cast<std::uint8_t>(playerId)
    };

    for (auto& client : clients)
        client.connection.queueMessage(msg);
}

void GameServer::broadcastAllZones()
{
    for (const auto& player : lobby.getPlayers())
    {
        Protocol::Message msg = {
            static_cast<std::uint8_t>(
                MessageType::PlayerZoneChanged
            ),
            static_cast<std::uint8_t>(
                player.playerId
            ),
            player.zoneId,
            player.secondaryZoneId,
            player.thirdZoneId
        };

        for (auto& client : clients)
            client.connection.queueMessage(msg);
    }
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
            break;

        if (gameState != GameState::Lobby)
            break;

        std::uint8_t zoneId = message[1];

        if (!lobby.setPlayerZone(
            client.playerId, zoneId))
        {
            std::cout
                << "Zona "
                << static_cast<int>(zoneId)
                << " no disponible para jugador "
                << client.playerId
                << "\n";

            break;
        }

        std::cout
            << "Jugador "
            << client.playerId
            << " selecciono Zona "
            << static_cast<int>(zoneId)
            << "\n";

        broadcastAllZones();
        break;
    }


    case MessageType::StartGameRequest:
    {
        if (message.size() != 1)
            break;

        if (gameState != GameState::Lobby)
            break;

        // Solo el Host puede iniciar.
        if (!lobby.isHost(client.playerId))
        {
            sendStartGameRejected(client);
            break;
        }

        // Todos los jugadores conectados deben
        // haber elegido una zona principal.
        if (!lobby.haveAllPlayersSelectedZone())
        {
            std::cout
                << "No se puede iniciar: "
                << "hay jugadores sin zona\n";

            sendStartGameRejected(client);
            break;
        }

        // Las tres zonas deben estar cubiertas.
        if (!lobby.areAllZonesCovered())
        {
            std::cout
                << "No se puede iniciar: "
                << "faltan zonas por cubrir\n";

            sendStartGameRejected(client);
            break;
        }

        // Todas las condiciones se cumplieron.
        gameState = GameState::Playing;

        std::cout
            << "Partida iniciada por jugador "
            << client.playerId
            << "\n";

        broadcastGameStarted();

        break;
    }


    default:
        break;
    }
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


void GameServer::broadcastGameStarted()
{
    Protocol::Message msg = {
        static_cast<std::uint8_t>(
            MessageType::GameStarted
        )
    };

    for (auto& client : clients)
    {
        client.connection.queueMessage(msg);
    }

    std::cout
        << "Inicio de partida enviado "
        << "a todos los jugadores\n";
}

void GameServer::sendStartGameRejected(
    ClientSession& client)
{
    Protocol::Message msg = {
        static_cast<std::uint8_t>(
            MessageType::StartGameRejected
        )
    };

    client.connection.queueMessage(msg);

    std::cout
        << "Solicitud de inicio rechazada "
        << "para jugador "
        << client.playerId
        << "\n";
}
