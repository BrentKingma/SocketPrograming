#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <thread>
#include <map>
#include <queue>
#include <vector>

#include "Client.h"
#include "Message.h"

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "25565"

namespace AdvancedWinsock
{

	class Server
	{
	private:
		WSAData wsaData;

		SOCKET listenSocket;

		int iResult;

		bool runServer;

		std::thread listenThread;
		std::thread messageHandler;

		std::vector<int> currentOnlineIDs;

		std::map<int, Client*> clients;

		std::queue<Message> messagesToSort;

		struct addrinfo* result, * ptr, hints;
	public:
		Server();
		~Server();

		void Tick();

		int Initialize();
		int Create(const char* port);
		int StartListen();
		int StopListen();

	private:
		int RegisterClient(UINT_PTR socket, SOCKADDR_IN socketInfo);
	};
}

