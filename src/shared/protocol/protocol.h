
#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace Protocol
{
    constexpr std::uint32_t MAX_MESSAGE_SIZE = 64 * 1024;

    using Byte = std::uint8_t;
    using Message = std::vector<Byte>;

    // Agrega un encabezado de 4 bytes.
    Message encode(const Message& payload);

    // Extrae un mensaje completo del buffer.
    // Devuelve false si todavía faltan bytes.
    bool decode(
        Message& receiveBuffer,
        Message& output
    );
}
