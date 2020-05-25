#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <queue>
#include <string>
#include "Misc.h"
#include "Message.h"

#pragma once

#define DEFAULT_BUFFLEN 144

namespace AdvancedWinsock
{
	class Client
	{
	public:
		Client();
		Client(SOCKET socket, int clientID);
		~Client();

		SOCKET mySocket;
		SOCKADDR_IN myInfo;
		int id;

		bool disconnected;

		void Registered();
		void addToQueue(std::string message);
		void Reconnect(SOCKET socket);
		void Disconnect();

		bool ContainMessage();
		Message GetReceivedMessage();

	private:
		bool isActive;

		int recvResult;
		int sendResult;

		char recvbuf[DEFAULT_BUFFLEN];

		std::string messageTemp;

		std::queue<std::string> sendMessageQueue;
		std::queue<Message> messagesToSend;

		std::thread receiver;
		std::thread sender;

		void StartThreads();
	};
}