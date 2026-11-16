#pragma once


#include <SFML/Graphics.hpp>

#include <string>

class Button
{
private:
    sf::RectangleShape shape;
    sf::Text label;

    sf::Color normalColor;
    sf::Color hoverColor;
    sf::Color disabledColor;

    bool enabled = true;

public:
    Button(
        const sf::Font& font,
        const std::string& text,
        sf::Vector2f position,
        sf::Vector2f size,
        sf::Color color
    );

    void setEnabled(bool value);

    bool isEnabled() const;

    bool contains(sf::Vector2f point) const;

    void update(sf::Vector2f mousePosition);

    void draw(sf::RenderWindow& window) const;
};
