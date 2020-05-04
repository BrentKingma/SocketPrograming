#include "Client.h"
namespace WinsockAdvanced
{
	Client::Client()
	{
		iResult = 0;
		connectSocket = INVALID_SOCKET;
		result = NULL;
		ptr = NULL;
		runThreads = true;
		sendBuffer = std::queue<const char*>();
	}
	Client::~Client()
	{

	}

	void Client::AddMessage(const char* message)
	{
		sendBuffer.push(message);
	}

	int Client::Initialize()
	{
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0)
		{
			printf("Winsock 2.2 not avaliable.");
			return 1;
		}
		return 0;
	}
	int Client::Create(const char* ip, const char* port)
	{
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		//Resolve server address and port
		iResult = getaddrinfo(ip, port, &hints, &result);
		if (iResult != 0)
		{
			printf("getaddrinfo failed %d\n", iResult);
			WSACleanup();
			return 1;
		}

		ptr = result;

		connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (connectSocket == INVALID_SOCKET)
		{
			printf("Error at socket(): %d\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}
		return 0;
	}
	int Client::Connect()
	{
		iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(connectSocket);
			connectSocket = INVALID_SOCKET;
		}

		freeaddrinfo(result);

		if (connectSocket == INVALID_SOCKET)
		{
			printf("Unable to connect to server!\n");
			//WSACleanup();
			return 1;
		}

		printf("Connected!\n");
		//using lambda as this function is a non-static function
		sendThread = std::thread([this] {this->SendThread(); });
		recvThread = std::thread([this] {this->ReceiveThread(); });
	}
	int Client::Disconnect()
	{
		runThreads = false;
		//shutdown sending module
		iResult = shutdown(connectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR)
		{
			printf("shutdown failed: %d\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			return 1;
		}
		//shutdown receiving module
		iResult = shutdown(connectSocket, SD_RECEIVE);
		if (iResult == SOCKET_ERROR)
		{
			printf("shutdown failed: %d\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			return 1;
		}
		//cleanup
		closesocket(connectSocket);
		WSACleanup();
		sendThread.join();
		recvThread.join();
		return 0;
	}
	void Client::SendThread()
	{
		int sendResult;
		do
		{
			if (!sendBuffer.empty())
			{
				const char* sendbuf = sendBuffer.front();
				sendBuffer.pop();
				sendResult = send(connectSocket, sendbuf, (int)strlen(sendbuf), 0);
				if (sendResult == SOCKET_ERROR)
				{
					printf("send failed: %d\n", WSAGetLastError());
					closesocket(connectSocket);
					WSACleanup();
				}
			}
			else
			{
				//std::this_thread::sleep_for(std::chrono::seconds(5));
			}
		} while (runThreads);
	}
	void Client::ReceiveThread()
	{
		int receiveResult;
		int recvbuflen = DEFAULT_BUFFER_LEN;
		char recvbuf[DEFAULT_BUFFER_LEN];
		do
		{
			receiveResult = recv(connectSocket, recvbuf, recvbuflen, 0);
			if (receiveResult > 0)
			{
				printf("Bytes received: %d\n", recvbuf);
			}
			else if (receiveResult == 0)
			{
				printf("Connection closed\n");
			}
			else
			{
				int error = WSAGetLastError();
				if (error == WSAEINTR)
				{

				}
				else
				{
					printf("Receive failed: %d\n", error);
				}
			}
		} while (receiveResult > 0 && runThreads);
	}
}