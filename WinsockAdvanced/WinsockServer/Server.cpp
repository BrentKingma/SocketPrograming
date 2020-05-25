#include "Server.h"

namespace AdvancedWinsock
{
	Server::Server()
	{
		listenSocket = INVALID_SOCKET;
		result = NULL;
		ptr = NULL;
		runServer = true;

		clients = std::map<int, Client*>();

		currentOnlineIDs = std::vector<int>();

		messagesToSort = std::queue<Message>();
	}
	Server::~Server()
	{

	}

	void Server::Tick()
	{
		if (clients.size() > 0)
		{
			for (auto const& x : clients)
			{
				if (x.second->disconnected)
				{
					printf("Client has disconnected\n");
					x.second->Disconnect();
					int id = x.first;
					currentOnlineIDs.erase(std::remove(currentOnlineIDs.begin(), currentOnlineIDs.end(), id));
					break; //One disconnect per tick
				}
				if (x.second->ContainMessage())
				{
					messagesToSort.push(x.second->GetReceivedMessage());
				}
			}
		}
	}

	int Server::Initialize()
	{
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0)
		{
			printf("WSAStartup failed: %d\n", iResult);
			return 1;
		}
		return 0;
	}

	int Server::Create(const char* port)
	{
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		iResult = getaddrinfo(NULL, (port != NULL) ? port : DEFAULT_PORT, &hints, &result);
		if (iResult != 0)
		{
			printf("getaddrinfo failed: %d\n", iResult);
			WSACleanup();
			return 1;
		}

		listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

		if (listenSocket == INVALID_SOCKET)
		{
			printf("Error at socket(): %ld\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}

		// Setup the TCP listening socket
		iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			printf("bind failed with error: %d\n", WSAGetLastError());
			freeaddrinfo(result);
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}

		freeaddrinfo(result);

		if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
		{
			printf("Listen failed with error: %ld\n", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}
		return 0;
	}

	int Server::StartListen()
	{
		//Sort through messages from clients
		messageHandler = std::thread([this]() {
			while (runServer)
			{
				if (messagesToSort.size() > 0)
				{
					if (messagesToSort.front().command != "")
					{
						std::string command = messagesToSort.front().command;
						//Commands
						if (command == "!online")
						{
							std::string message;
							for (int i = 0; i < currentOnlineIDs.size(); i++)
							{
								if (currentOnlineIDs[i] != messagesToSort.front().fromID)
								{
									message += std::to_string(currentOnlineIDs[i]) + "\n";
								}
							}
							clients[messagesToSort.front().fromID]->addToQueue(message);
						}
						if (command == "!isClient")
						{
							bool exists = clients.find(std::stoi(messagesToSort.front().message)) != clients.end() ? true : false;
							clients[messagesToSort.front().fromID]->addToQueue(exists ? "Exists" : "Doesn't Exist");
						}
					}
					else
					{
						std::string message = std::to_string(messagesToSort.front().fromID) + ": " + messagesToSort.front().message;
						clients[messagesToSort.front().toID]->addToQueue(message);
					}
					messagesToSort.pop();
				}
			}
			});
		listenThread = std::thread([this]() {
			while (runServer)
			{
				SOCKET clientSocket = INVALID_SOCKET;
				SOCKADDR_IN client_info = { 0 };
				int addresSize = sizeof(client_info);
				clientSocket = accept(listenSocket, (struct sockaddr*)&client_info, &addresSize);
				printf("Registring Client\n");
				RegisterClient(clientSocket, client_info);
			}
			});
		return 0;
	}

	int Server::StopListen()
	{
		runServer = false;
		closesocket(listenSocket);
		listenThread.join();

		//for (std::map<int, std::thread>::value_type& x : clientReceiverThreads)
		//{
		//	x.second.join();
		//}
		//clientReceiverThreads.clear();

		//for (std::map<int, std::thread>::value_type& x : clientSenderThreads)
		//{
		//	x.second.join();
		//}
		//clientSenderThreads.clear();

		WSACleanup();

		result = NULL;
		ptr = NULL;

		return 0;
	}

	int Server::RegisterClient(UINT_PTR socket, SOCKADDR_IN socketInfo)
	{
		int id = ((int)socketInfo.sin_addr.S_un.S_un_b.s_b1 * (int)socketInfo.sin_addr.S_un.S_un_b.s_b2 + (int)socketInfo.sin_addr.S_un.S_un_b.s_b3 * (int)socketInfo.sin_addr.S_un.S_un_b.s_b4) * 2 - 72;
		if (clients.find(id) == clients.end())
		{
			printf("Client has not been registered before, create new intake\n");
			//Client has not been registered
			Client* client = new Client(socket, id);			
			clients[client->id] = client;
			client->Registered();
		}
		else
		{
			printf("Client has been registered before, reconnecting\n");
			clients[id]->Reconnect(socket);
		}
		currentOnlineIDs.push_back(id);
		return 0;
	}	
}
