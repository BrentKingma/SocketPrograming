#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <thread>
#include <queue>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFFER_LEN 144

namespace WinsockAdvanced
{
	class Client
	{
	private:
		WSAData wsaData;

		struct addrinfo* result, * ptr, hints;

		SOCKET connectSocket;

		int iResult;

		std::queue<std::string> sendBuffer;

		bool runThreads;

		std::thread sendThread;
		std::thread recvThread;
	public:
		Client();
		~Client();

		void AddMessage(std::string message);

		int Initialize();
		int Create(std::string ip, const char* port);
		int Connect();
		int Disconnect();

		void SendThread();
		void ReceiveThread();
	};
}
