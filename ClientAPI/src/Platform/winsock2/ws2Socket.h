#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "Core/Socket.h"

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

class ws2Socket : public Socket
{
public:
	virtual void Init(const SocketProps& props) override;

	virtual void Connect(const std::string& ip, int port) override;
	virtual void Disconnect() override;

	virtual void Send(const std::string& msg) override;
	virtual std::string Receive() override;

private:
	SOCKET m_SocketDescriptor = INVALID_SOCKET;
	bool m_IsConnected = false;
};