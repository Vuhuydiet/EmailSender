#include "app_pch.h"
#include "app_pch.h"
#include "UserInput.h"

static const std::set<std::string> s_AppCommands = { "/quit", "/clr", "/begin", "/end"};
static const std::set<std::string> s_ServerCommands = { "/send" };

static bool IsAppCommand(const std::string& str) 
{
	return s_AppCommands.find(str) != s_AppCommands.end();
}

static bool IsServerCommand(const std::string& str) 
{
	return s_ServerCommands.find(str) != s_ServerCommands.end();
}

static UserInput::Type IsCommand(const std::string& str)
{
	if (IsAppCommand(str))
		return UserInput::Type::AppCommand;
	if (IsServerCommand(str))
		return UserInput::Type::ServerCommand;
	return UserInput::Type::Message;
}


UserInput UserInput::Get() 
{
	std::string input;
	std::getline(std::cin, input);

	UserInput ret;
	std::stringstream ss(input);
	std::string cmd;
	ss >> cmd;
	ret.m_Type = IsCommand(cmd);
	if (ret.m_Type == Type::Message)
	{
		ret.m_Msg = ss.str();
	}
	else
	{
		ret.m_Command = cmd;
		// 1 additional offset for 1 space after the command
		if (cmd.size() + 1 < input.size())
			ret.m_Msg = ss.str().substr(cmd.size() + 1, input.size() - (cmd.size() + 1));
	}

	return ret;
}