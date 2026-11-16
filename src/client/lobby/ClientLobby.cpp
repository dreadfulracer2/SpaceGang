
#include "ClientLobby.h"

#include <algorithm>
#include <iostream>
#include <string>

void ClientLobby::clear()
{
    players.clear();
}

void ClientLobby::setPlayers(
    const std::vector<PlayerInfo>& newPlayers)
{
    players = newPlayers;
}

void ClientLobby::addPlayer(
    std::uint32_t playerId)
{
    for (const auto& player : players)
    {
        if (player.playerId == playerId)
            return;
    }

    PlayerInfo player;

    player.playerId = playerId;

    player.name =
        "Jugador " + std::to_string(playerId);

    player.zoneId = 0;
    player.secondaryZoneId = 0;
    player.thirdZoneId = 0;

    player.isHost = (playerId == 1);

    players.push_back(player);
}

void ClientLobby::removePlayer(
    std::uint32_t playerId)
{
    players.erase(
        std::remove_if(
            players.begin(),
            players.end(),
            [playerId](const PlayerInfo& player)
            {
                return player.playerId == playerId;
            }
        ),
        players.end()
    );
}

void ClientLobby::updateZone(
    std::uint32_t playerId,
    std::uint8_t zoneId,
    std::uint8_t secondaryZoneId,
    std::uint8_t thirdZoneId)
{
    for (auto& player : players)
    {
        if (player.playerId == playerId)
        {
            player.zoneId = zoneId;
            player.secondaryZoneId = secondaryZoneId;
            player.thirdZoneId = thirdZoneId;

            return;
        }
    }
}

const std::vector<PlayerInfo>&
ClientLobby::getPlayers() const
{
    return players;
}

void ClientLobby::printPlayers() const
{
    std::cout << "\n--- LOBBY ---\n";

    for (const auto& player : players)
    {
        std::cout
            << "Jugador " << player.playerId
            << " | Principal: "
            << static_cast<int>(player.zoneId);

        if (player.secondaryZoneId != 0)
        {
            std::cout
                << " | Secundaria: "
                << static_cast<int>(
                    player.secondaryZoneId
                    );
        }

        if (player.thirdZoneId != 0)
        {
            std::cout
                << " | Tercera: "
                << static_cast<int>(
                    player.thirdZoneId
                    );
        }

        if (player.isHost)
            std::cout << " | HOST";

        std::cout << "\n";
    }

    std::cout << "-------------\n";
}
