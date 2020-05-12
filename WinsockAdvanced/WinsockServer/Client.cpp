#include "Client.h"

namespace AdvancedWinsock
{
	Client::Client(){}
	Client::Client(SOCKET socket, SOCKADDR_IN info)
	{
		mySocket = socket;
		myInfo = info;
		sendMessageQueue = std::queue<const char*>();
		isActive = true;
		registered = false;
		id = ((int)myInfo.sin_addr.S_un.S_un_b.s_b1 * (int)myInfo.sin_addr.S_un.S_un_b.s_b2 + (int)myInfo.sin_addr.S_un.S_un_b.s_b3 * (int)myInfo.sin_addr.S_un.S_un_b.s_b4) * 2 - 72;

		const char* idChar = IntToChar(id);

		sendMessageQueue.push(idChar);
	}

	Client::~Client()
	{
		closesocket(mySocket);
		if (registered)
		{
			receiver.join();
			sender.join();
		}
	}

	void Client::Registered()
	{
		printf("Client ID %d Joined\n", id);
		registered = true;
		
		StartThreads();
	}

	void Client::addToQueue(const char* message)
	{
		sendMessageQueue.push(message);
	}

	void Client::Reconnect(SOCKET socket, SOCKADDR_IN info)
	{
		Disconnect();

		mySocket = socket;
		myInfo = info;

		StartThreads();
	}

	void Client::Disconnect()
	{
		closesocket(mySocket);
		receiver.join();
		sender.join();
	}

	void Client::StartThreads()
	{
		//Receiver Thread
		receiver = std::thread([this]() {
			while (isActive)
			{
				ZeroMemory(recvbuf, DEFAULT_BUFFLEN);
				recvResult = recv(mySocket, recvbuf, DEFAULT_BUFFLEN, 0);
				if (recvResult > 0)
				{
					//Cleans message
					for (int i = DEFAULT_BUFFLEN - 1; i >= 0; i--)
					{
						if (recvbuf[i] == 'Ì')
						{
							recvbuf[i] = NULL;
						}
					}
					printf("Message: %s\n", recvbuf);
				}
				else if (recvResult == 0)
				{
					printf("ID: %d disconnected\n", id);

				}
				else
				{
					printf("recv failed: %d\n", WSAGetLastError());
				}
			}
			});
		//Sender Thread
		sender = std::thread([this]() {
			while (isActive)
			{
				//Only send if queue contains something
				if (!sendMessageQueue.empty())
				{
					sendResult = send(mySocket, sendMessageQueue.front(), sizeof(sendMessageQueue.front()), 0);
					if (sendResult == SOCKET_ERROR)
					{
						printf("send failed: %d\n", WSAGetLastError());
					}
					else
					{
						printf("message sent: %d\n", sendResult);
						sendMessageQueue.pop();
					}
				}
				else
				{
					std::this_thread::sleep_for(std::chrono::seconds(1));
				}
			}
			});
	}
}
