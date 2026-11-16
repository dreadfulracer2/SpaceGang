
#include "Game.h"

#include <iostream>

Game::Game()
    : window(
        sf::VideoMode({ 1280, 720 }),
        "SpaceGang"
    )
{
    window.setFramerateLimit(60);
}

bool Game::connectToServer(const std::string& ip)
{
    if (!network.connectToServer(ip))
    {
        std::cout << "No se pudo conectar al servidor\n";
        return false;
    }

    std::cout << "Conexion establecida correctamente\n";

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
        }
    }
}

void Game::update()
{
    if (!network.update())
    {
        std::cout << "Conexion con el servidor finalizada\n";

        running = false;
        window.close();
    }
}

void Game::render()
{
    window.clear(sf::Color(25, 28, 38));

    // Aqui dibujaremos los elementos del juego.

    window.display();
}
