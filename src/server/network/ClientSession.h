
#pragma once

#include "network/TcpConnection.h"

#include <cstdint>

struct ClientSession
{
    std::uint32_t playerId = 0;
    TcpConnection connection;

    explicit ClientSession(std::uint32_t id)
        : playerId(id)
    {
    }
};
