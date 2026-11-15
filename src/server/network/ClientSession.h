#pragma once

#pragma once

#include <SFML/Network.hpp>
#include <cstdint>

struct ClientSession
{
    std::uint32_t playerId;
    sf::TcpSocket socket;

    explicit ClientSession(std::uint32_t id)
        : playerId(id)
    {
    }
};
