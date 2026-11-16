
#pragma once

#include <SFML/Graphics.hpp>

#include "network/NetworkClient.h"
#include "ui/Button.h"
#include <memory>
#include <cstdint>
#include <memory>
#include <string>
#include "ui/RocketVotingScreen.h"

class Game
{
private:
    sf::RenderWindow window;
    NetworkClient network;

    sf::Font font;

    std::unique_ptr<Button> zoneAButton;
    std::unique_ptr<Button> zoneBButton;
    std::unique_ptr<Button> zoneCButton;

    bool running = true;
    std::unique_ptr<RocketVotingScreen> rocketVotingScreen;

public:
    Game();

    bool connectToServer(const std::string& ip);

    void run();

private:
    void processEvents();
    void update();
    void render();

    void updateZoneButtons();

    void handleMouseClick(sf::Vector2f position);

    void drawText(
        const std::string& text,
        float x,
        float y,
        unsigned int size,
        sf::Color color
    );

    void drawPlayerCard(
        std::uint32_t playerId,
        float x,
        float y
    );

    void selectZone(std::uint8_t zoneId);
    std::unique_ptr<Button> startGameButton;
    bool isLocalPlayerHost() const;
    void renderPlaying();
};
