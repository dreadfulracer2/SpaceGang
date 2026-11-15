#pragma once
#include <cstdint>
#include <string>

struct PlayerInfo
{
    std::uint32_t playerId = 0;
    std::string name;
    std::uint8_t zoneId = 0;
    bool isHost = false;
};
