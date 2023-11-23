#include "pch.h"
#include "Socket.h"

#include "Platform/winsock2/ws2Socket.h"

Ref<Socket> Socket::Create()
{
	Ref<Socket> socket = CreateRef<ws2Socket>();
	return socket;
}