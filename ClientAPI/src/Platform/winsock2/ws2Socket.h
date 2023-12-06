#pragma once
#include "Core/Socket.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
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

	virtual void SendFile(const std::string& path) override;
	virtual void ReceiveFile(const std::string& dst_path) override;

private:
	SOCKET m_SocketDescriptor = INVALID_SOCKET;
	bool m_IsConnected = false;
};