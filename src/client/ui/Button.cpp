
#include "Button.h"
#include <algorithm>
#include <cstdint>
Button::Button(
    const sf::Font& font,
    const std::string& text,
    sf::Vector2f position,
    sf::Vector2f size,
    sf::Color color
)
    : label(font, text, 20),
    normalColor(color),
    hoverColor(
        sf::Color(
            static_cast<std::uint8_t>(
                std::min(255, color.r + 35)
                ),
            static_cast<std::uint8_t>(
                std::min(255, color.g + 35)
                ),
            static_cast<std::uint8_t>(
                std::min(255, color.b + 35)
                )
        )
    ),
    disabledColor(65, 70, 82)
{
    shape.setPosition(position);
    shape.setSize(size);
    shape.setFillColor(normalColor);

    label.setFillColor(sf::Color::White);

    auto bounds = label.getLocalBounds();

    label.setOrigin({
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f
        });

    label.setPosition({
        position.x + size.x / 2.f,
        position.y + size.y / 2.f
        });
}

void Button::setEnabled(bool value)
{
    enabled = value;
}

bool Button::isEnabled() const
{
    return enabled;
}

bool Button::contains(sf::Vector2f point) const
{
    return enabled &&
        shape.getGlobalBounds().contains(point);
}

void Button::update(sf::Vector2f mousePosition)
{
    if (!enabled)
    {
        shape.setFillColor(disabledColor);
        return;
    }

    if (contains(mousePosition))
        shape.setFillColor(hoverColor);
    else
        shape.setFillColor(normalColor);
}

void Button::draw(sf::RenderWindow& window) const
{
    window.draw(shape);
    window.draw(label);
}
