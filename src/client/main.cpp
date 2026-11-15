
#include "network/NetworkClient.h"
#include <iostream>

int main()
{
    NetworkClient client;

    std::string ip;

    std::cout << "IP del servidor: ";
    std::cin >> ip;

    if (!client.connectToServer(ip))
    {
        std::cout << "No se pudo conectar\n";
        return 1;
    }

    client.receiveMessages();

    return 0;
}
