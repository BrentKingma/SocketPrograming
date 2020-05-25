#include "Client.h"

namespace AdvancedWinsock
{
	Client::Client()
	{
		mySocket = INVALID_SOCKET;
		sendMessageQueue = std::queue<std::string>();
		isActive = false;
		disconnected = false;
		id = 0;
	}
	Client::Client(SOCKET socket, int clientID)
	{
		mySocket = socket;
		sendMessageQueue = std::queue<std::string>();
		messagesToSend = std::queue<Message>();
		isActive = true;
		disconnected = false;
		id = clientID;

		std::string idChar = IntToString(id);

		sendMessageQueue.push(idChar);
	}

	Client::~Client()
	{
		
	}

	void Client::Registered()
	{
		printf("Client ID %d Joined\n", id);
		
		StartThreads();
	}

	void Client::addToQueue(std::string message)
	{
		sendMessageQueue.push(message);
	}

	void Client::Reconnect(SOCKET socket)
	{
		mySocket = socket;

		isActive = true;
		disconnected = false;

		StartThreads();
	}

	void Client::Disconnect()
	{
		isActive = false;

		closesocket(mySocket);
		
		mySocket = INVALID_SOCKET;

		if (receiver.joinable())
		{
			receiver.join();
		}
		if (sender.joinable())
		{
			sender.join();
		}
		disconnected = false;
	}

	void Client::StartThreads()
	{
		//Receiver Thread
		receiver = std::thread([this]() {
			while (isActive)
			{
				if (mySocket != INVALID_SOCKET && !disconnected)
				{
					ZeroMemory(recvbuf, DEFAULT_BUFFLEN);
					recvResult = recv(mySocket, recvbuf, DEFAULT_BUFFLEN, 0);
					printf("Receiveing message: %s\n", recvbuf);
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
						messageTemp = recvbuf;
						if (messageTemp[0] == '!')
						{
							std::string command = messageTemp.substr(0, messageTemp.find("#"));
							messageTemp.erase(0, messageTemp.find("#") + 1);
							Message message = Message(id, 0, command, messageTemp);
							messagesToSend.push(message);
						}
						else
						{
							int toId = std::stoi(messageTemp.substr(0, messageTemp.find("#")));
							messageTemp.erase(0, messageTemp.find("#") + 1);
							Message message = Message(id, toId, "", messageTemp);
							messagesToSend.push(message);
						}
						printf("Message: %s\n", recvbuf);
					}
					else if (recvResult == 0)
					{
						printf("ID: %d disconnected\n", id);
						if (disconnected == false)
						{
							disconnected = true;
						}
					}
					else
					{
						printf("recv failed: %d\n", WSAGetLastError());
						if (disconnected == false)
						{
							disconnected = true;
						}
					}
				}
			}
			});
		//Sender Thread
		sender = std::thread([this]() {
			while (isActive)
			{
				if (mySocket != INVALID_SOCKET && !disconnected)
				{
					//Only send if queue contains something
					if (!sendMessageQueue.empty())
					{
						const char* message = sendMessageQueue.front().c_str();
						sendResult = send(mySocket, message, DEFAULT_BUFFLEN, 0);
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
			}
			});
	}

	bool Client::ContainMessage()
	{
		if (messagesToSend.size() > 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	Message Client::GetReceivedMessage()
	{
		auto temp = messagesToSend.front();
		messagesToSend.pop();
		return temp;
	}
}
