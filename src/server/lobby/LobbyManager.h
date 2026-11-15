
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

    std::array<std::optional<PlayerInfo>, MAX_PLAYERS>
        players;

public:
    std::optional<std::uint32_t> addPlayer(
        const std::string& name
    );

    bool removePlayer(std::uint32_t playerId);

    bool setPlayerZone(
        std::uint32_t playerId,
        std::uint8_t zoneId
    );

    std::vector<PlayerInfo> getPlayers() const;

    bool isFull() const;
    std::size_t getPlayerCount() const;
};
