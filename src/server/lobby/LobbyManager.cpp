
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
            player.secondaryZoneId = 0;
            player.thirdZoneId = 0;
            player.isHost = (i == 0);

            players[i] = player;

            updateHostSecondaryZone();

            return player.playerId;
        }
    }

    return std::nullopt;
}

bool LobbyManager::removePlayer(
    std::uint32_t playerId)
{
    if (playerId < 1 || playerId > MAX_PLAYERS)
        return false;

    auto& slot = players[playerId - 1];

    if (!slot.has_value())
        return false;

    slot.reset();

    updateHostSecondaryZone();

    return true;
}

bool LobbyManager::canSelectZone(
    std::uint32_t playerId,
    std::uint8_t zoneId) const
{
    if (zoneId < 1 || zoneId > 3)
        return false;

    if (playerId < 1 || playerId > MAX_PLAYERS)
        return false;

    if (!players[playerId - 1].has_value())
        return false;

    const int capacities[4] = { 0, 1, 2, 1 };

    int occupied = 0;

    for (const auto& slot : players)
    {
        if (!slot.has_value())
            continue;

        // No contar al jugador que solicita el cambio.
        if (slot->playerId == playerId)
            continue;

        if (slot->zoneId == zoneId)
            ++occupied;
    }

    return occupied < capacities[zoneId];
}

bool LobbyManager::setPlayerZone(
    std::uint32_t playerId,
    std::uint8_t zoneId)
{
    if (!canSelectZone(playerId, zoneId))
        return false;

    auto& player = players[playerId - 1];

    player->zoneId = zoneId;

    updateHostSecondaryZone();

    return true;
}

void LobbyManager::updateHostSecondaryZone()
{
    PlayerInfo* host = nullptr;

    for (auto& slot : players)
    {
        if (slot.has_value() && slot->isHost)
        {
            host = &slot.value();
            break;
        }
    }

    if (!host)
        return;

    host->secondaryZoneId = 0;
    host->thirdZoneId = 0;

    // El Host primero debe elegir su zona principal.
    if (host->zoneId == 0)
        return;

    std::uint8_t freeZones[2] = { 0, 0 };
    int freeCount = 0;

    for (std::uint8_t zone = 1; zone <= 3; ++zone)
    {
        if (zone == host->zoneId)
            continue;

        bool occupied = false;

        for (const auto& slot : players)
        {
            if (!slot.has_value())
                continue;

            if (slot->zoneId == zone)
            {
                occupied = true;
                break;
            }
        }

        if (!occupied && freeCount < 2)
        {
            freeZones[freeCount] = zone;
            ++freeCount;
        }
    }

    if (freeCount >= 1)
        host->secondaryZoneId = freeZones[0];

    if (freeCount >= 2)
        host->thirdZoneId = freeZones[1];
}

std::vector<PlayerInfo>
LobbyManager::getPlayers() const
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

bool LobbyManager::areAllZonesCovered() const
{
    bool covered[4] = { false, false, false, false };

    for (const auto& player : getPlayers())
    {
        if (player.zoneId >= 1 && player.zoneId <= 3)
            covered[player.zoneId] = true;

        if (player.secondaryZoneId >= 1 &&
            player.secondaryZoneId <= 3)
        {
            covered[player.secondaryZoneId] = true;
        }

        if (player.thirdZoneId >= 1 &&
            player.thirdZoneId <= 3)
        {
            covered[player.thirdZoneId] = true;
        }
    }

    return covered[1] && covered[2] && covered[3];
}

bool LobbyManager::isHost(std::uint32_t playerId) const
{
    if (playerId < 1 || playerId > MAX_PLAYERS)
        return false;

    const auto& slot = players[playerId - 1];

    return slot.has_value() && slot->isHost;
}

bool LobbyManager::haveAllPlayersSelectedZone() const
{
    // Comprobar que todos los jugadores conectados
    // hayan seleccionado una zona principal.

    for (const auto& slot : players)
    {
        if (!slot.has_value())
            continue;

        if (slot->zoneId == 0)
            return false;
    }

    return true;
}
