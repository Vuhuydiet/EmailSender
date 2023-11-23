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
	virtual void Init(const SocketProps& props) = 0;

	virtual void Connect(const std::string& ip, int port) = 0;
	virtual void Disconnect() = 0;

	virtual void Send(const std::string& msg) = 0;
	virtual std::string Receive() = 0;

	static Ref<Socket> Create();
protected:
	int m_Af;
	int m_Type;
	int m_Protocol;
};