#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <thread>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFFER_LEN 512

namespace WinsockAdvanced
{
	class Client
	{
	private:
		WSAData wsaData;

		struct addrinfo* result, * ptr, hints;

		SOCKET connectSocket;

		int iResult;

		std::vector<const char*> sendBuffer;

		bool runThreads;

		std::thread sendThread;
		std::thread recvThread;
	public:
		Client();
		~Client();

		void AddMessage(const char* message);

		int Initialize();
		int Create(const char* ip, const char* port);
		int Connect();
		int Disconnect();

		void SendThread();
		void ReceiveThread();
	};
}
