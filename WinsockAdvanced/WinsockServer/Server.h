#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <thread>
#include <map>
#include <queue>
#include <string>
#include <sstream>

#include "Client.h"

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "25565"
#define DEFAULT_BUFFLEN 144

namespace AdvancedWinsock
{

	class Server
	{
	private:
		WSAData wsaData;

		SOCKET listenSocket;

		int iResult;
		int recvbuflen;

		bool runServer;

		std::thread listenThread;
		std::thread clientRegisterThread;

		std::map<int, Client*> clients;
		std::map<int, std::thread> clientReceiverThreads;
		std::map<int, std::thread> clientSenderThreads;
		std::map<int, std::queue<const char*>> clientSendQueues;

		std::queue<int>idQueue;
		std::queue<Client*>clientsToRegister;

		struct addrinfo* result, * ptr, hints;
	public:
		Server();
		~Server();

		void StartClientRecvThread(int id);
		void StartClientSendThread(int id);

		int Initialize();
		int Create(const char* port);
		int StartListen();
		int StopListen();

	private:
		int RegisterClient(Client* client);
	};
}

