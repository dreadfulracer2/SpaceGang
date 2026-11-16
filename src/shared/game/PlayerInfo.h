
#pragma once

#include <cstdint>
#include <string>

struct PlayerInfo
{
    std::uint32_t playerId = 0;
    std::string name;

    // Zona principal
    std::uint8_t zoneId = 0;

    // Zonas adicionales
    std::uint8_t secondaryZoneId = 0;
    std::uint8_t thirdZoneId = 0;

    bool isHost = false;
};
