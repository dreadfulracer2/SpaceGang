
#include "LobbyManager.h"

std::optional<std::uint32_t>
LobbyManager::addPlayer(const std::string& name)
{
    for (std::size_t i = 0; i < players.size(); ++i)
    {
        if (!players[i].has_value())
        {
            PlayerInfo player;

            player.playerId =
                static_cast<std::uint32_t>(i + 1);

            player.name = name;
            player.zoneId = 0;
            player.isHost = (i == 0);

            players[i] = player;

            return player.playerId;
        }
    }

    return std::nullopt;
}

bool LobbyManager::removePlayer(std::uint32_t playerId)
{
    if (playerId < 1 || playerId > MAX_PLAYERS)
        return false;

    auto& slot = players[playerId - 1];

    if (!slot.has_value())
        return false;

    slot.reset();

    return true;
}

bool LobbyManager::setPlayerZone(
    std::uint32_t playerId,
    std::uint8_t zoneId)
{
    if (playerId < 1 || playerId > MAX_PLAYERS)
        return false;

    if (zoneId < 1 || zoneId > 3)
        return false;

    auto& slot = players[playerId - 1];

    if (!slot.has_value())
        return false;

    slot->zoneId = zoneId;

    return true;
}

std::vector<PlayerInfo> LobbyManager::getPlayers() const
{
    std::vector<PlayerInfo> result;

    for (const auto& player : players)
    {
        if (player.has_value())
            result.push_back(*player);
    }

    return result;
}

bool LobbyManager::isFull() const
{
    return getPlayerCount() == MAX_PLAYERS;
}

std::size_t LobbyManager::getPlayerCount() const
{
    std::size_t count = 0;

    for (const auto& player : players)
    {
        if (player.has_value())
            ++count;
    }

    return count;
}
