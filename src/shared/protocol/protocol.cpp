
#include "Protocol.h"

#include <stdexcept>

namespace Protocol
{
    Message encode(const Message& payload)
    {
        if (payload.empty() ||
            payload.size() > MAX_MESSAGE_SIZE)
        {
            throw std::length_error(
                "Tamano de mensaje invalido"
            );
        }

        const auto size =
            static_cast<std::uint32_t>(payload.size());

        Message result;
        result.reserve(4 + payload.size());

        // Encabezado en orden de red (big-endian).
        result.push_back(
            static_cast<Byte>((size >> 24) & 0xFF)
        );

        result.push_back(
            static_cast<Byte>((size >> 16) & 0xFF)
        );

        result.push_back(
            static_cast<Byte>((size >> 8) & 0xFF)
        );

        result.push_back(
            static_cast<Byte>(size & 0xFF)
        );

        result.insert(
            result.end(),
            payload.begin(),
            payload.end()
        );

        return result;
    }

    bool decode(
        Message& receiveBuffer,
        Message& output)
    {
        if (receiveBuffer.size() < 4)
            return false;

        const std::uint32_t size =
            (std::uint32_t(receiveBuffer[0]) << 24) |
            (std::uint32_t(receiveBuffer[1]) << 16) |
            (std::uint32_t(receiveBuffer[2]) << 8) |
            std::uint32_t(receiveBuffer[3]);

        if (size == 0 || size > MAX_MESSAGE_SIZE)
        {
            throw std::runtime_error(
                "Encabezado de mensaje invalido"
            );
        }

        const std::size_t totalSize = 4 + size;

        if (receiveBuffer.size() < totalSize)
            return false;

        output.assign(
            receiveBuffer.begin() + 4,
            receiveBuffer.begin() + totalSize
        );

        receiveBuffer.erase(
            receiveBuffer.begin(),
            receiveBuffer.begin() + totalSize
        );

        return true;
    }
}
