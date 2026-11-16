
#include "ConsoleInput.h"

#include <iostream>

ConsoleInput::~ConsoleInput()
{
    // El hilo puede estar esperando en std::getline.
    // Para esta prueba, el proceso termina desde main.
    if (inputThread.joinable())
        inputThread.detach();
}

void ConsoleInput::start()
{
    if (running.exchange(true))
        return;

    inputThread = std::thread([this]()
        {
            std::string line;

            while (running && std::getline(std::cin, line))
            {
                std::lock_guard<std::mutex> lock(mutex);
                commands.push(line);
            }

            running = false;
        });
}

bool ConsoleInput::popCommand(std::string& command)
{
    std::lock_guard<std::mutex> lock(mutex);

    if (commands.empty())
        return false;

    command = std::move(commands.front());
    commands.pop();

    return true;
}
