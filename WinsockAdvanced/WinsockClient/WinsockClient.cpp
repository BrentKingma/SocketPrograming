#include "Client.h"

int main()
{
	char sentence[140];
	bool run = true;

	WinsockAdvanced::Client client;
	if (client.Initialize() != 0)
	{
		
	}
	if (client.Create("192.168.1.151", "22556") != 0)
	{

	}
	while (client.Connect() != 0)
	{
		printf("Retrying\n");
		if (client.Create("192.168.1.151", "22556") != 0)
		{
			return 1;
		}
	}
	while (run)
	{
		if (fgets(sentence, 140, stdin)[0] == 'd')
		{
			run = false;
			client.Disconnect();
		}
		else
		{
			int size = strlen(sentence) - 1;
			if (sentence[size] == '\n')
			{
				sentence[size] = '\0';
			}
			client.AddMessage(sentence);
		}
	}
}