
#pragma once

#include <SFML/Graphics.hpp>
#include "network/NetworkClient.h"

#include <string>

class Game
{
private:
    sf::RenderWindow window;
    NetworkClient network;

    bool running = true;

public:
    Game();

    bool connectToServer(const std::string& ip);

    void run();

private:
    void processEvents();
    void update();
    void render();
};
