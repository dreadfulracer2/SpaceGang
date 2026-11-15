
#include "TcpConnection.h"

#include <algorithm>
#include <stdexcept>

sf::TcpSocket& TcpConnection::getSocket()
{
    return socket;
}

void TcpConnection::setConnected(bool value)
{
    connected = value;

    if (connected)
        socket.setBlocking(false);
}

bool TcpConnection::isConnected() const
{
    return connected;
}

bool TcpConnection::queueMessage(
    const Protocol::Message& message)
{
    if (!connected)
        return false;

    auto encoded = Protocol::encode(message);

    if (encoded.size() >
        MAX_PENDING_BYTES - pendingBytes)
    {
        return false;
    }

    pendingBytes += encoded.size();
    sendQueue.push_back(std::move(encoded));

    return true;
}

bool TcpConnection::flushSendQueue()
{
    if (!connected)
        return false;

    while (!sendQueue.empty())
    {
        auto& current = sendQueue.front();

        const std::size_t remaining =
            current.size() - sendOffset;

        std::size_t sent = 0;

        auto status = socket.send(
            current.data() + sendOffset,
            remaining,
            sent
        );

        sendOffset += sent;
        pendingBytes -= sent;

        if (sendOffset == current.size())
        {
            sendQueue.pop_front();
            sendOffset = 0;
        }

        if (status == sf::Socket::Status::NotReady ||
            status == sf::Socket::Status::Partial)
        {
            return true;
        }

        if (status != sf::Socket::Status::Done)
        {
            disconnect();
            return false;
        }

        if (sent == 0)
            return true;
    }

    return true;
}

bool TcpConnection::receiveMessages(
    std::vector<Protocol::Message>& messages)
{
    if (!connected)
        return false;

    std::array<std::uint8_t, 4096> buffer{};

    // Evita procesar una cantidad ilimitada de
    // datos de un solo cliente en cada actualizacion.
    constexpr int MAX_READS = 16;

    for (int i = 0; i < MAX_READS; ++i)
    {
        std::size_t received = 0;

        auto status = socket.receive(
            buffer.data(),
            buffer.size(),
            received
        );

        if (received > 0)
        {
            receiveBuffer.insert(
                receiveBuffer.end(),
                buffer.begin(),
                buffer.begin() + received
            );

            try
            {
                Protocol::Message message;

                while (Protocol::decode(
                    receiveBuffer, message))
                {
                    messages.push_back(
                        std::move(message)
                    );

                    message.clear();
                }
            }
            catch (const std::exception&)
            {
                disconnect();
                return false;
            }
        }

        if (status == sf::Socket::Status::NotReady)
            return true;

        if (status == sf::Socket::Status::Disconnected ||
            status == sf::Socket::Status::Error)
        {
            disconnect();
            return false;
        }

        if (status != sf::Socket::Status::Done &&
            status != sf::Socket::Status::Partial)
        {
            disconnect();
            return false;
        }

        // Limite adicional de seguridad.
        if (receiveBuffer.size() >
            Protocol::MAX_MESSAGE_SIZE + 4)
        {
            disconnect();
            return false;
        }
    }

    return true;
}

void TcpConnection::disconnect()
{
    socket.disconnect();
    connected = false;

    receiveBuffer.clear();
    sendQueue.clear();

    sendOffset = 0;
    pendingBytes = 0;
}
