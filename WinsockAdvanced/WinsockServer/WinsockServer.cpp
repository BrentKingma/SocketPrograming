#include "Server.h"

int main()
{
    AdvancedWinsock::Server server;

    server.Initialize();
    server.Create("22556");
    server.StartListen();

    while (true)
    {
        server.Tick();
    }
}

