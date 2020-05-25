#include <string>
#pragma once

namespace AdvancedWinsock
{
	class Message
	{
	public: 
		Message()
		{
			fromID = 0;
			toID = 0;
		};
		Message(int a_fromID, int a_toID, std::string a_command, std::string a_message)
		{
			fromID = a_fromID;
			toID = a_toID;
			command = a_command;
			message = a_message;
		}

		int fromID;
		int toID;
		std::string command;
		std::string message;
	};
}

