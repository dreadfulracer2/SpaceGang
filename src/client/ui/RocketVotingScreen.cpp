
#include "RocketVotingScreen.h"

#include <array>
#include <string>

namespace
{
    struct DisplayComponent
    {
        const char* name;
        int price;
        int mass;
        int thrust;
    };

    // Datos provisionales.
    constexpr std::array<std::array<DisplayComponent, 3>, 3>
        components = { {
            {{
                {"Cabina A", 120, 800, 0},
                {"Cabina B", 160, 1100, 0},
                {"Cabina C", 210, 1400, 0}
            }},
            {{
                {"Motor A", 90, 600, 18},
                {"Motor B", 130, 850, 26},
                {"Motor C", 180, 1100, 34}
            }},
            {{
                {"Motor D", 75, 500, 15},
                {"Motor E", 115, 700, 22},
                {"Motor F", 155, 950, 30}
            }}
        } };

    constexpr std::array<const char*, 3> categoryNames = {
        "CREW MODULE",
        "FIRST STAGE",
        "PROPULSOR 2"
    };
}

RocketVotingScreen::RocketVotingScreen(sf::Font& font)
    : font(font)
{
    // Distribucion para ventana de 1280 x 720.
    for (int category = 0; category < 3; ++category)
    {
        for (int option = 0; option < 3; ++option)
        {
            optionBounds[category][option] = sf::FloatRect(
                { 80.f + option * 390.f,
                 115.f + category * 165.f },
                { 340.f, 115.f }
            );
        }
    }

    confirmBounds = sf::FloatRect(
        { 430.f, 625.f },
        { 420.f, 60.f }
    );
}

void RocketVotingScreen::reset()
{
    selections = { -1, -1, -1 };
}

bool RocketVotingScreen::allSelected() const
{
    for (int selection : selections)
    {
        if (selection < 0)
            return false;
    }

    return true;
}

void RocketVotingScreen::handleClick(
    const sf::Vector2f& position,
    NetworkClient& network)
{
    if (network.hasConfirmedRocketVote() ||
        network.isRocketVotePending() ||
        network.isRocketVotingFinished())
    {
        return;
    }

    // Seleccionar una opcion por categoria.
    for (int category = 0; category < 3; ++category)
    {
        for (int option = 0; option < 3; ++option)
        {
            if (optionBounds[category][option].contains(position))
            {
                selections[category] = option;
                return;
            }
        }
    }

    // Confirmar las tres selecciones.
    if (confirmBounds.contains(position) && allSelected())
    {
        network.submitRocketVote(
            static_cast<std::uint8_t>(selections[0]),
            static_cast<std::uint8_t>(selections[1]),
            static_cast<std::uint8_t>(selections[2])
        );
    }
}

void RocketVotingScreen::drawText(
    sf::RenderWindow& window,
    const std::string& content,
    unsigned int size,
    const sf::Vector2f& position,
    const sf::Color& color)
{
    sf::Text text(font, content, size);
    text.setPosition(position);
    text.setFillColor(color);
    window.draw(text);
}

void RocketVotingScreen::render(
    sf::RenderWindow& window,
    const NetworkClient& network)
{
    window.clear(sf::Color(16, 22, 34));

    drawText(
        window,
        "SPACEGANG - CREW MODULE",
        29,
        { 80.f, 25.f },
        sf::Color::White
    );

    const bool locked =
        network.hasConfirmedRocketVote() ||
        network.isRocketVotePending() ||
        network.isRocketVotingFinished();

    for (int category = 0; category < 3; ++category)
    {
        drawText(
            window,
            categoryNames[category],
            18,
            { 80.f, 83.f + category * 165.f },
            sf::Color(190, 200, 220)
        );

        for (int option = 0; option < 3; ++option)
        {
            const auto& bounds =
                optionBounds[category][option];

            const auto& component =
                components[category][option];

            sf::RectangleShape card(bounds.size);
            card.setPosition(bounds.position);

            const bool selected =
                selections[category] == option;

            card.setFillColor(
                selected
                ? sf::Color(28, 83, 72)
                : sf::Color(37, 47, 65)
            );

            card.setOutlineThickness(
                selected ? 3.f : 1.f
            );

            card.setOutlineColor(
                selected
                ? sf::Color(65, 220, 150)
                : sf::Color(90, 105, 130)
            );

            window.draw(card);

            const float x = bounds.position.x + 15.f;
            const float y = bounds.position.y + 8.f;

            drawText(
                window,
                component.name,
                19,
                { x, y },
                sf::Color::White
            );

            drawText(
                window,
                "Price: $" + std::to_string(component.price),
                15,
                { x, y + 32.f },
                sf::Color(210, 220, 230)
            );

            drawText(
                window,
                "Mass: " + std::to_string(component.mass) + " kg",
                15,
                { x, y + 56.f },
                sf::Color(210, 220, 230)
            );

            if (category != 0)
            {
                drawText(
                    window,
                    "Empuje: " +
                    std::to_string(component.thrust) + " kN",
                    15,
                    { x + 160.f, y + 56.f },
                    sf::Color(210, 220, 230)
                );
            }
        }
    }

    // Contador de jugadores.
    drawText(
        window,
        "Votes Received: " +
        std::to_string(network.getRocketVotesReceived()) +
        " / " +
        std::to_string(network.getRocketTotalPlayers()),
        17,
        { 80.f, 585.f },
        sf::Color(190, 200, 220)
    );

    sf::RectangleShape confirm(confirmBounds.size);
    confirm.setPosition(confirmBounds.position);

    const bool canConfirm = allSelected() && !locked;

    confirm.setFillColor(
        canConfirm
        ? sf::Color(65, 110, 220)
        : sf::Color(65, 70, 85)
    );

    window.draw(confirm);

    std::string buttonText = "CONFIRMAR MIS 3 VOTOS";

    if (network.isRocketVotePending())
        buttonText = "SUBMITTING VOTES...";
    else if (network.hasConfirmedRocketVote())
        buttonText = "VOTES CONFIRMED";
    else if (network.isRocketVotingFinished())
        buttonText = "VOTACION FINALIZADA";

    drawText(
        window,
        buttonText,
        19,
        { confirmBounds.position.x + 45.f,
         confirmBounds.position.y + 16.f },
        sf::Color::White
    );

    if (network.isRocketVotingFinished())
    {
        const auto& winners = network.getRocketWinners();

        drawText(
            window,
            "Winning Components: " +
            std::to_string(winners[0] + 1) + " / " +
            std::to_string(winners[1] + 1) + " / " +
            std::to_string(winners[2] + 1),
            16,
            { 80.f, 675.f },
            sf::Color(100, 230, 160)
        );
    }

    window.display();
}
