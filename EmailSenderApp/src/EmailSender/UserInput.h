#pragma once

#include <string>

class UserInput 
{
public:
	enum class Type { None, AppCommand, ServerCommand, Message };

	Type GetType() const { return m_Type; }
	const std::string& GetCommand() const { return m_Command; }
	const std::string& GetMsg() const { return m_Msg; }
	std::string GetInput() const { return m_Command + ' ' + m_Msg; }
	
	static UserInput Get();

private:
	Type m_Type = Type::None;
	std::string m_Command;
	std::string m_Msg;
};