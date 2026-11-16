
#pragma once

#include <atomic>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

class ConsoleInput
{
private:
    std::thread inputThread;
    std::mutex mutex;
    std::queue<std::string> commands;

    std::atomic<bool> running{ false };

public:
    ConsoleInput() = default;

    ~ConsoleInput();

    void start();
    bool popCommand(std::string& command);
};
