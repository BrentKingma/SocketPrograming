#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "22556"
#define DEFAULT_BUFFLEN 512

int main()
{
	WSADATA wsaData;
	int iResult;

	//Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed %d\n", iResult);
		return 1;
	}
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//Resolve server address and port
	iResult = getaddrinfo("192.168.1.151", DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed %d\n", iResult);
		WSACleanup();
		return 1;
	}

	SOCKET connectSocket = INVALID_SOCKET;

	//Attempt to connect to the first address returned by the call to getaddrinfo
	ptr = result;

	//Create socket for connecting to the server
	connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	//Check for errors
	if (connectSocket == INVALID_SOCKET)
	{
		printf("Error at socket(): %d\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	//Connect to the server
	iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(connectSocket);
		connectSocket = INVALID_SOCKET;
	}

	freeaddrinfo(result);

	if (connectSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!");
		WSACleanup();
		return 1;
	}

	int recvbuflen = DEFAULT_BUFFLEN;

	const char* sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFFLEN];

	iResult = send(connectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR)
	{
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes sent: %ld\n", iResult);

	//shutdown the connection for sending since no more data will be sent, receiving data will still work
	iResult = shutdown(connectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	//Receive data until server closes connection
	do
	{
		iResult = recv(connectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			printf("Bytes received: %d\n", iResult);
		}
		else if (iResult == 0)
		{
			printf("Connection closed\n");
		}
		else
		{
			printf("Receive failed: %d\n", WSAGetLastError());
		}
	} while (iResult > 0);

	iResult = shutdown(connectSocket, SD_RECEIVE);
	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}
	
	//cleanupp
	closesocket(connectSocket);
	WSACleanup();
	system("PAUSE");
	return 0;
}