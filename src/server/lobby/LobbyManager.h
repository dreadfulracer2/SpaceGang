
#pragma once

#include "game/PlayerInfo.h"

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

class LobbyManager
{
private:
    static constexpr std::size_t MAX_PLAYERS = 4;

    std::array<
        std::optional<PlayerInfo>,
        MAX_PLAYERS
    > players;

public:
    std::optional<std::uint32_t> addPlayer(
        const std::string& name
    );

    bool removePlayer(std::uint32_t playerId);

    bool setPlayerZone(
        std::uint32_t playerId,
        std::uint8_t zoneId
    );

    bool canSelectZone(
        std::uint32_t playerId,
        std::uint8_t zoneId
    ) const;

    void updateHostSecondaryZone();

    std::vector<PlayerInfo> getPlayers() const;

    bool isFull() const;

    std::size_t getPlayerCount() const;
    bool areAllZonesCovered() const;
    bool isHost(std::uint32_t playerId) const;
    bool haveAllPlayersSelectedZone() const;
};
