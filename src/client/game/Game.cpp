
#include "Game.h"

#include <iostream>
#include <stdexcept>
#include <string>


Game::Game()
    : window(
        sf::VideoMode({ 1280, 720 }),
        "SpaceGang"
    )
{
    window.setFramerateLimit(60);

    if (!font.openFromFile(
        "assets/fonts/DejaVuSans.ttf"))
    {
        throw std::runtime_error(
            "No se pudo cargar la fuente"
        );
    }

    zoneAButton = std::make_unique<Button>(
        font,
        "ZONA A",
        sf::Vector2f(80.f, 590.f),
        sf::Vector2f(340.f, 65.f),
        sf::Color(37, 99, 235)
    );

    zoneBButton = std::make_unique<Button>(
        font,
        "ZONA B",
        sf::Vector2f(470.f, 590.f),
        sf::Vector2f(340.f, 65.f),
        sf::Color(22, 163, 74)
    );

    zoneCButton = std::make_unique<Button>(
        font,
        "ZONA C",
        sf::Vector2f(860.f, 590.f),
        sf::Vector2f(340.f, 65.f),
        sf::Color(217, 119, 6)
    );

    startGameButton = std::make_unique<Button>(
        font,
        "INICIAR PARTIDA",
        sf::Vector2f(450.f, 665.f),
        sf::Vector2f(380.f, 45.f),
        sf::Color(120, 65, 200)
    );

}


bool Game::connectToServer(
    const std::string& ip)
{
    if (!network.connectToServer(ip))
    {
        std::cout
            << "No se pudo conectar al servidor\n";

        return false;
    }

    return true;
}

void Game::run()
{
    while (window.isOpen() && running)
    {
        processEvents();

        if (!running || !window.isOpen())
            break;

        update();

        if (!running || !window.isOpen())
            break;

        render();
    }

    network.disconnect();
}


void Game::processEvents()
{
    while (const auto event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            running = false;
            window.close();
        }

        if (const auto* key =
            event->getIf<sf::Event::KeyPressed>())
        {
            if (key->code == sf::Keyboard::Key::Escape)
            {
                running = false;
                window.close();
            }
            else if (
                key->code == sf::Keyboard::Key::Num1 ||
                key->code == sf::Keyboard::Key::Numpad1)
            {
                if (network.getGameState() == GameState::Lobby &&
                    zoneAButton->isEnabled())
                {
                    selectZone(1);
                }
            }
            else if (
                key->code == sf::Keyboard::Key::Num2 ||
                key->code == sf::Keyboard::Key::Numpad2)
            {
                if (network.getGameState() == GameState::Lobby &&
                    zoneAButton->isEnabled())
                {
                    selectZone(2);
                }
            }
            else if (
                key->code == sf::Keyboard::Key::Num3 ||
                key->code == sf::Keyboard::Key::Numpad3)
            {
                if (network.getGameState() == GameState::Lobby &&
                    zoneAButton->isEnabled())
                {
                    selectZone(3);
                }
            }
        }

        if (const auto* mouse =
            event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouse->button == sf::Mouse::Button::Left)
            {
                auto position = window.mapPixelToCoords(
                    mouse->position
                );

                handleMouseClick(position);
            }
        }
    }
}

void Game::handleMouseClick(sf::Vector2f position)
{

    if (network.getGameState() != GameState::Lobby)
        return;

    if (isLocalPlayerHost() &&
        startGameButton->contains(position))
    {
        if (network.requestStartGame())
        {
            std::cout
                << "Solicitando inicio de partida\n";
        }

        return;
    }

    if (zoneAButton->contains(position))
    {
        selectZone(1);
    }
    else if (zoneBButton->contains(position))
    {
        selectZone(2);
    }
    else if (zoneCButton->contains(position))
    {
        selectZone(3);
    }
}

void Game::updateZoneButtons()
{
    int occupied[4] = { 0, 0, 0, 0 };

    std::uint8_t myZone = 0;

    for (const auto& player :
        network.getLobby().getPlayers())
    {
        if (player.zoneId >= 1 &&
            player.zoneId <= 3)
        {
            ++occupied[player.zoneId];
        }

        if (player.playerId == network.getPlayerId())
        {
            myZone = player.zoneId;
        }
    }

    const int capacity[4] = { 0, 1, 2, 1 };

    // El jugador puede conservar su zona actual.
    zoneAButton->setEnabled(
        occupied[1] < capacity[1] || myZone == 1
    );

    zoneBButton->setEnabled(
        occupied[2] < capacity[2] || myZone == 2
    );

    zoneCButton->setEnabled(
        occupied[3] < capacity[3] || myZone == 3
    );

    auto mousePosition = window.mapPixelToCoords(
        sf::Mouse::getPosition(window)
    );

    zoneAButton->update(mousePosition);
    zoneBButton->update(mousePosition);
    zoneCButton->update(mousePosition);
}


void Game::selectZone(std::uint8_t zoneId)
{
    if (network.selectZone(zoneId))
    {
        std::cout
            << "Solicitando zona "
            << static_cast<int>(zoneId)
            << "\n";
    }
    else
    {
        std::cout
            << "No se pudo solicitar la zona\n";
    }
}


void Game::update()
{
    if (!network.update())
    {
        running = false;
        window.close();
        return;
    }

    updateZoneButtons();

    if (network.getGameState() == GameState::Lobby)
    {
        startGameButton->setEnabled(
            isLocalPlayerHost()
        );

        startGameButton->update(
            window.mapPixelToCoords(
                sf::Mouse::getPosition(window)
            )
        );
    }

    if (network.consumeStartGameRejected())
    {
        std::cout
            << "No se cumplen las condiciones "
            << "para iniciar la partida\n";
    }

}


void Game::drawText(
    const std::string& text,
    float x,
    float y,
    unsigned int size,
    sf::Color color)
{
    sf::Text label(font, text, size);

    label.setFillColor(color);
    label.setPosition({ x, y });

    window.draw(label);
}

void Game::drawPlayerCard(
    std::uint32_t playerId,
    float x,
    float y)
{
    sf::RectangleShape card({ 500.f, 180.f });

    card.setPosition({ x, y });
    card.setFillColor(sf::Color(37, 46, 64));

    window.draw(card);

    const PlayerInfo* player = nullptr;

    for (const auto& p :
        network.getLobby().getPlayers())
    {
        if (p.playerId == playerId)
        {
            player = &p;
            break;
        }
    }

    if (player &&
        player->playerId == network.getPlayerId())
    {
        card.setOutlineThickness(3.f);
        card.setOutlineColor(
            sf::Color(96, 165, 250)
        );

        window.draw(card);
    }

    drawText(
        "SLOT " + std::to_string(playerId),
        x + 25,
        y + 15,
        18,
        sf::Color(150, 160, 180)
    );

    if (!player)
    {
        drawText(
            "ESPERANDO JUGADOR...",
            x + 25,
            y + 65,
            23,
            sf::Color(120, 130, 150)
        );

        return;
    }

    std::string name = player->isHost
        ? "HOST"
        : "JUGADOR " +
        std::to_string(player->playerId);

    drawText(
        name,
        x + 25,
        y + 60,
        27,
        sf::Color::White
    );

    std::string zoneText = "SIN ZONA";

    sf::Color zoneColor(160, 160, 160);

    switch (player->zoneId)
    {
    case 1:
        zoneText = "ZONA A";
        zoneColor = sf::Color(96, 165, 250);
        break;

    case 2:
        zoneText = "ZONA B";
        zoneColor = sf::Color(74, 222, 128);
        break;

    case 3:
        zoneText = "ZONA C";
        zoneColor = sf::Color(251, 191, 36);
        break;
    }

    drawText(
        zoneText,
        x + 25,
        y + 120,
        20,
        zoneColor
    );

    std::string additionalZones;

    if (player->secondaryZoneId != 0)
    {
        additionalZones += std::to_string(
            player->secondaryZoneId
        );
    }

    if (player->thirdZoneId != 0)
    {
        if (!additionalZones.empty())
            additionalZones += ", ";

        additionalZones += std::to_string(
            player->thirdZoneId
        );
    }

    if (!additionalZones.empty())
    {
        drawText(
            "ADICIONALES: " + additionalZones,
            x + 200,
            y + 120,
            15,
            sf::Color(200, 180, 255)
        );
    }
}

void Game::render()
{
    if (network.getGameState() == GameState::Playing)
    {
        renderPlaying();
        return;
    }
    window.clear(sf::Color(21, 27, 41));

    drawText(
        "SPACEGANG",
        80,
        35,
        42,
        sf::Color::White
    );

    drawText(
        "MULTIPLAYER LOBBY",
        82,
        95,
        18,
        sf::Color(150, 160, 180)
    );

    auto playerCount =
        network.getLobby().getPlayers().size();

    drawText(
        std::to_string(playerCount) +
        " / 4 JUGADORES",
        950,
        60,
        20,
        sf::Color(74, 222, 128)
    );

    drawPlayerCard(1, 80, 150);
    drawPlayerCard(2, 700, 150);

    drawPlayerCard(3, 80, 365);
    drawPlayerCard(4, 700, 365);


    zoneAButton->draw(window);
    zoneBButton->draw(window);
    zoneCButton->draw(window);

    drawText(
        "ESC: SALIR",
        1050,
        675,
        16,
        sf::Color(150, 160, 180)
    );
    if (isLocalPlayerHost())
    {
        startGameButton->draw(window);
    }
    window.display();
}

bool Game::isLocalPlayerHost() const
{
    for (const auto& player :
        network.getLobby().getPlayers())
    {
        if (player.playerId == network.getPlayerId())
            return player.isHost;
    }

    return false;
}

void Game::renderPlaying()
{
    window.clear(sf::Color(12, 18, 30));

    drawText(
        "SPACEGANG",
        470,
        180,
        48,
        sf::Color::White
    );

    drawText(
        "PARTIDA INICIADA",
        435,
        290,
        32,
        sf::Color(74, 222, 128)
    );

    drawText(
        "Jugador " +
        std::to_string(network.getPlayerId()),
        520,
        365,
        24,
        sf::Color::White
    );

    drawText(
        "ESC: SALIR",
        535,
        620,
        20,
        sf::Color(150, 160, 180)
    );

    window.display();
}
