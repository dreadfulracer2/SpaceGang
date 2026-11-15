
#include "GameServer.h"

int main()
{
    GameServer server;

    if (!server.start())
        return 1;

    server.run();
    server.stop();

    return 0;
}
