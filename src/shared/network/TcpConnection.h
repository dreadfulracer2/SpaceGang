
#pragma once

#include "protocol/Protocol.h"

#include <SFML/Network.hpp>

#include <array>
#include <cstddef>
#include <deque>
#include <vector>

class TcpConnection
{
private:
    sf::TcpSocket socket;

    Protocol::Message receiveBuffer;

    std::deque<Protocol::Message> sendQueue;
    std::size_t sendOffset = 0;
    std::size_t pendingBytes = 0;

    static constexpr std::size_t MAX_PENDING_BYTES =
        1024 * 1024;

    bool connected = false;

public:
    sf::TcpSocket& getSocket();

    void setConnected(bool value);

    bool isConnected() const;

    bool queueMessage(
        const Protocol::Message& message
    );

    // Devuelve false si ocurre una desconexion
    // o un error de comunicacion.
    bool flushSendQueue();

    bool receiveMessages(
        std::vector<Protocol::Message>& messages
    );

    void disconnect();
};
