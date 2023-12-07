#pragma once

#include <string>

#include "Core/Config.h"

struct SocketProps
{
	int af = -1;
	int type = -1;
	int protocol = -1;

	SocketProps() = default;
	SocketProps(int af, int type, int protocol) : af(af), type(type), protocol(protocol) {}
};

class Socket
{
public:
	Socket(const SocketProps& props);
	Socket(int af, int type, int protocol);
	virtual ~Socket() = default;

	virtual void Connect(const std::string& ip, int port) = 0;
	virtual void Disconnect() = 0;

	virtual void Send(const std::string& msg) = 0;
	virtual void Receive(std::string& buffer) = 0;

	static void Init();
	static Ref<Socket> Create(const SocketProps& props);

protected:
	int m_Af;
	int m_Type;
	int m_Protocol;
};