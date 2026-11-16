
#pragma once

#include "game/PlayerInfo.h"

#include <cstdint>
#include <vector>

class ClientLobby
{
private:
    std::vector<PlayerInfo> players;

public:
    void clear();

    void setPlayers(
        const std::vector<PlayerInfo>& newPlayers
    );

    void addPlayer(std::uint32_t playerId);

    void removePlayer(std::uint32_t playerId);

    void updateZone(
        std::uint32_t playerId,
        std::uint8_t zoneId,
        std::uint8_t secondaryZoneId,
        std::uint8_t thirdZoneId
    );

    const std::vector<PlayerInfo>& getPlayers() const;

    void printPlayers() const;
};
