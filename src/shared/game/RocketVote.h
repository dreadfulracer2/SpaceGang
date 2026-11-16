
#pragma once

#include <array>
#include <cstdint>

struct RocketVote
{
    std::uint32_t playerId = 0;

    // Cada posicion corresponde a una categoria:
    // 0 = Cabina
    // 1 = Propulsor 1
    // 2 = Propulsor 2

    // Cada valor es una opcion de 0 a 2.
    std::array<std::uint8_t, 3> selections = {
        0, 0, 0
    };

    bool confirmed = false;
};
