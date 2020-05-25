#include "Client.h"

int main()
{
	char sentence[144];
	bool run = true;

	char ipBuf[60];
	printf("Enter IP: ");
	fgets(ipBuf, 60, stdin);

	for (int i = 0; i < 60; i++)
	{
		if (ipBuf[i] == '\n')
		{
			ipBuf[i] = '\0';
			break;
		}
	}	

	std::string ip = ipBuf;

	WinsockAdvanced::Client client;
	if (client.Initialize() != 0)
	{
		
	}
	if (client.Create(ip, "22556") != 0)
	{

	}
	while (client.Connect() != 0)
	{
		printf("Retrying\n");
		if (client.Create(ip, "22556") != 0)
		{
			return 1;
		}
	}
	while (run)
	{
		fgets(sentence, 144, stdin);
		size_t size = strlen(sentence) - 1;
		sentence[size] = '\0';
		std::string temp = sentence;
		
		//Checks commands
		if (temp == "!help")
		{
			printf("Commands: \n!online			-	Pings server to see who is online\n!isClient#CLIENTNUM	-	Check if specific client is registered to server\n!d			-	Disconnect\nCLIENTNUM#MESSAGE	-	Send message to client\n");
		}
		else if (temp == "!d")
		{
			run = false;
			client.Disconnect();
		}
		else
		{		
			char* send = new(char[size]);
			memcpy(send, temp.c_str(), size);

			client.AddMessage(send);
		}
	}
}