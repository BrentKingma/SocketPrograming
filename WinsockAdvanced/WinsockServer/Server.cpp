#include "Server.h"

namespace AdvancedWinsock
{
	Server::Server()
	{
		listenSocket = INVALID_SOCKET;
		result = NULL;
		ptr = NULL;
		runServer = true;
		recvbuflen = DEFAULT_BUFFLEN;

		clients = std::map<int, Client*>();
		clientReceiverThreads = std::map<int, std::thread>();
		clientSenderThreads = std::map<int, std::thread>();
		clientSendQueues = std::map<int, std::queue<const char*>>();
	}
	Server::~Server()
	{

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
		clientRegisterThread = std::thread([this]() {
			while (runServer)
			{
				if (!clientsToRegister.empty())
				{

				}
				else
				{
					std::this_thread::sleep_for(std::chrono::seconds(1));
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

				Client* client = new Client(clientSocket, client_info);
				
				RegisterClient(client);
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

	int Server::RegisterClient(Client* client)
	{
		if (clients.find(client->id) == clients.end())
		{
			//Client has not been registered
			clients[client->id] = client;
			client->Registered();
		}
		else
		{
			//clients[client->id]->mySocket = client->mySocket;
			clients[client->id]->Reconnect(client->mySocket, client->myInfo);
			delete client;
		}
		//clients[id] = client;
		//clientSendQueues[id] = std::queue<const char*>();
		//clientReceiverThreads[id] = std::thread([this, id]() { StartClientRecvThread(id); });			
		//clientSenderThreads[id] = std::thread([this, id]() { StartClientSendThread(id); });
		return 0;
	}

	//void Server::StartClientSendThread(int id)
	//{
	//	int sendResult;
	//	SOCKET mySocket = clients[id];
	//	sendResult = send(mySocket, (char*)id, sizeof(id), 0);
	//	do
	//	{
	//		if (!clientSendQueues[id].empty())
	//		{
	//			// Echo the buffer back to the sender
	//			sendResult = send(mySocket, clientSendQueues[id].front(), sizeof(clientSendQueues[id].front()), 0);
	//			if (sendResult == SOCKET_ERROR)
	//			{
	//				printf("send failed: %d\n", WSAGetLastError());
	//			}
	//			else
	//			{
	//				clientSendQueues[id].pop();
	//			}
	//		}
	//		else
	//		{
	//			std::this_thread::sleep_for(std::chrono::seconds(1));
	//		}
	//	}
	//	while (runServer);
	//}

	//void Server::StartClientRecvThread(int id)
	//{
	//	char recvbuf[DEFAULT_BUFFLEN];
	//	int myID = id;
	//	SOCKET myClient = clients[myID];

	//	int recvResult;
	//	do
	//	{
	//		ZeroMemory(recvbuf, DEFAULT_BUFFLEN);
	//		recvResult = recv(myClient, recvbuf, recvbuflen, 0);
	//		if (recvResult > 0)
	//		{
	//			for (int i = DEFAULT_BUFFLEN - 1; i >= 0; i--)
	//			{
	//				if (recvbuf[i] == 'Ì')
	//				{
	//					recvbuf[i] = NULL;
	//				}
	//			}
	//			std::string idToSend = "";
	//			for (int i = 0; i < DEFAULT_BUFFLEN; i++)
	//			{
	//				if ((int)recvbuf[i] == '0' || (int)recvbuf[i] == '1' || (int)recvbuf[i] == '2' || (int)recvbuf[i] == '3' || (int)recvbuf[i] == '4' || (int)recvbuf[i] == '5' || (int)recvbuf[i] == '6' || (int)recvbuf[i] == '7' || (int)recvbuf[i] == '8' || (int)recvbuf[i] == '9')
	//				{
	//					idToSend += recvbuf[i];
	//					recvbuf[i] = NULL;
	//				}
	//				else if (recvbuf[i] == '#')
	//				{
	//					std::stringstream stream(idToSend);
	//					int c_id = 0;
	//					stream >> c_id;
	//					if (clients.find(c_id) != clients.end())
	//					{
	//						clientSendQueues[c_id].push(recvbuf);
	//						break;
	//					}
	//					else
	//					{
	//						clientSendQueues[myID].push("ID doesnt exist");
	//						break;
	//					}
	//				}
	//				else
	//				{
	//					clientSendQueues[myID].push("Incorrect ID");
	//					break;
	//				}
	//			}
	//		}
	//		else if (recvResult == 0)
	//		{
	//			printf("ID: %d disconnected\n", myID);
	//		}
	//		else
	//		{
	//			printf("recv failed: %d\n", WSAGetLastError());
	//			//closesocket(myClient);
	//			//WSACleanup();
	//		}

	//	}
	//	while (recvResult > 0 && runServer);
	//}
	
}
