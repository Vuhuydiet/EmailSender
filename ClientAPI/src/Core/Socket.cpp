#include "pch.h"
#include "Socket.h"

#include "Platform/WinSock2/WS2Socket.h"

Socket::Socket(const SocketProps& props)
	: m_Af(props.af), m_Type(props.type), m_Protocol(props.protocol)
{
}

Socket::Socket(SocketProps::AF af, SocketProps::Type type, SocketProps::Protocol protocol)
	: m_Af(af), m_Type(type), m_Protocol(protocol)
{
}

void Socket::Init()
{
	WS2Socket::Init();
}

void Socket::Shutdown() 
{
	WS2Socket::Shutdown();
}

Ref<Socket> Socket::Create(const SocketProps& props)
{
	return CreateRef<WS2Socket>(props);
}

Ref<Socket> Socket::Create(SocketProps::AF af, SocketProps::Type type, SocketProps::Protocol protocol)
{
	return CreateRef<WS2Socket>(af, type, protocol);
}