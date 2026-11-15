
#include "GameServer.h"
#include "protocol/MessageType.h"

#include <iostream>

bool GameServer::start()
{
    if (listener.listen(PORT) != sf::Socket::Status::Done)
        return false;

    listener.setBlocking(false);
    selector.add(listener);

    running = true;

    std::cout << "Servidor iniciado en puerto "
        << PORT << "\n";

    return true;
}

void GameServer::run()
{
    while (running)
    {
        if (!selector.wait(sf::milliseconds(100)))
            continue;

        if (selector.isReady(listener))
            acceptClient();

        checkClients();
    }
}

void GameServer::acceptClient()
{
    clients.emplace_back(nextPlayerId);

    auto& session = clients.back();

    if (listener.accept(session.socket)
        != sf::Socket::Status::Done)
    {
        clients.pop_back();
        return;
    }

    if (clients.size() > MAX_PLAYERS)
    {
        sf::Packet rejected;
        rejected << static_cast<std::uint8_t>(
            MessageType::ServerFull
            );

        session.socket.send(rejected);
        session.socket.disconnect();
        clients.pop_back();
        return;
    }

    session.socket.setBlocking(true);

    sf::Packet accepted;

    accepted << static_cast<std::uint8_t>(
        MessageType::JoinAccepted
        );

    accepted << nextPlayerId;

    if (session.socket.send(accepted)
        != sf::Socket::Status::Done)
    {
        session.socket.disconnect();
        clients.pop_back();
        return;
    }

    selector.add(session.socket);

    std::cout << "Jugador "
        << nextPlayerId
        << " conectado\n";

    broadcastPlayerJoined(nextPlayerId);

    ++nextPlayerId;
}

void GameServer::broadcastPlayerJoined(
    std::uint32_t playerId)
{
    sf::Packet packet;

    packet << static_cast<std::uint8_t>(
        MessageType::PlayerJoined
        );

    packet << playerId;

    for (auto& client : clients)
        client.socket.send(packet);
}

void GameServer::checkClients()
{
    for (auto it = clients.begin(); it != clients.end();)
    {
        if (!selector.isReady(it->socket))
        {
            ++it;
            continue;
        }

        sf::Packet packet;
        auto status = it->socket.receive(packet);

        if (status == sf::Socket::Status::Disconnected ||
            status == sf::Socket::Status::Error)
        {
            std::cout << "Jugador "
                << it->playerId
                << " desconectado\n";

            selector.remove(it->socket);
            it = clients.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void GameServer::stop()
{
    running = false;
    selector.clear();

    for (auto& client : clients)
        client.socket.disconnect();

    clients.clear();
    listener.close();
}
