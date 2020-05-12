#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <queue>
#include "Misc.h"

#pragma once

#define DEFAULT_BUFFLEN 144

namespace AdvancedWinsock
{
	class Client
	{
	public:
		Client();
		Client(SOCKET socket, SOCKADDR_IN info);
		~Client();

		SOCKET mySocket;
		SOCKADDR_IN myInfo;
		int id;

		void Registered();
		void addToQueue(const char* message);
		void Reconnect(SOCKET socket, SOCKADDR_IN info);
		void Disconnect();

	private:
		bool isActive;
		bool registered;

		int recvResult;
		int sendResult;

		char recvbuf[DEFAULT_BUFFLEN];

		std::queue<const char*> sendMessageQueue;

		std::thread receiver;
		std::thread sender;

		void StartThreads();
	};
}