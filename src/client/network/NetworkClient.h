
#pragma once

#include <SFML/Network.hpp>
#include <cstdint>
#include <string>

class NetworkClient
{
private:
    sf::TcpSocket socket;
    std::uint32_t playerId = 0;

public:
    bool connectToServer(const std::string& ip);
    void receiveMessages();
    void disconnect();

    std::uint32_t getPlayerId() const;
};
