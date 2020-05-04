#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <thread>
#include <map>
#include <queue>
#include <string>
#include <sstream>


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
		std::map<int, SOCKET> clients;
		std::map<int, std::thread> clientReceiverThreads;
		std::map<int, std::thread> clientSenderThreads;
		std::map<int, std::queue<const char*>> clientSendQueues;
		std::queue<int>idQueue;

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
		int AddClient(SOCKET client, SOCKADDR_IN clientInfo);
	};
}

