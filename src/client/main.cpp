
#include "game/Game.h"

#include <iostream>
#include <string>

int main()
{
    std::string ip;

    std::cout << "IP del servidor: ";
    std::cin >> ip;

    Game game;

    if (!game.connectToServer(ip))
        return 1;

    game.run();

    return 0;
}
