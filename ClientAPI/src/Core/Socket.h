#pragma once

#include <string>

#include "Core/Config.h"

struct SocketProps
{
	enum class AF { NONE, INET };
	enum class Type { NONE, SOCKSTREAM };
	enum class Protocol { NONE, IPPROTOCOL_TCP };

	AF af = AF::NONE;
	Type type = Type::NONE;
	Protocol protocol = Protocol::NONE;

	SocketProps() = default;
	SocketProps(AF af, Type type, Protocol protocol) : af(af), type(type), protocol(protocol) {}
};

class Socket
{
public:
	Socket(const SocketProps& props);
	Socket(SocketProps::AF af, SocketProps::Type type, SocketProps::Protocol protocol);
	virtual ~Socket() = default;

	virtual void Connect(const std::string& ip, int port) = 0;
	virtual void Disconnect() = 0;
	virtual bool IsConnected() const = 0;

	virtual void Send(const std::string& msg) = 0;
	virtual std::string Receive(size_t bytes = 0) = 0;
	virtual std::string Receive(const std::string& back_string) = 0;

	static void Init();
	static void Shutdown();
	static Ref<Socket> Create(const SocketProps& props);
	static Ref<Socket> Create(SocketProps::AF af, SocketProps::Type type, SocketProps::Protocol protocol);

protected:
	SocketProps::AF m_Af; 
	SocketProps::Type m_Type;
	SocketProps::Protocol m_Protocol;
};