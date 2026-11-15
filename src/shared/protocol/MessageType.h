
#pragma once

#include <cstdint>

enum class MessageType : std::uint8_t
{
    JoinAccepted = 1,
    PlayerJoined,
    PlayerLeft,
    ServerFull,
    ChatMessage,

    LobbyState,
    SelectZone,
    PlayerZoneChanged
};
