
#pragma once

#include <SFML/Graphics.hpp>

#include <array>
#include <cstdint>
#include <string>

#include "network/NetworkClient.h"

class RocketVotingScreen
{
private:
    sf::Font& font;

    // -1 significa que no se ha seleccionado.
    std::array<int, 3> selections = { -1, -1, -1 };

    std::array<std::array<sf::FloatRect, 3>, 3> optionBounds;

    sf::FloatRect confirmBounds;

public:
    explicit RocketVotingScreen(sf::Font& font);

    void handleClick(
        const sf::Vector2f& position,
        NetworkClient& network
    );

    void render(
        sf::RenderWindow& window,
        const NetworkClient& network
    );

    void reset();

private:
    bool allSelected() const;

    void drawText(
        sf::RenderWindow& window,
        const std::string& content,
        unsigned int size,
        const sf::Vector2f& position,
        const sf::Color& color
    );
};
