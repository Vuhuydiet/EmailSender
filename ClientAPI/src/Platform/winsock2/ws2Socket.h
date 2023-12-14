#pragma once
#include "Core/Socket.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

class WS2Socket : public Socket
{
public:
	WS2Socket(const SocketProps& props);
	WS2Socket(SocketProps::AF af, SocketProps::Type type, SocketProps::Protocol protocol);
	~WS2Socket();

	virtual void Connect(const std::string& ip, int port) override;
	virtual void Disconnect() override;
	virtual bool IsConnected() const override { return m_IsConnected; }

	virtual void Send(const std::string& msg) override;
	virtual std::string Receive(size_t bytes = 0) override;
	virtual std::string Receive(const std::string& back_string) override;
	
	static void Init();
	static void Shutdown();

private:
	void CreateNewSocket();
	void Delete();

private:
	SOCKET m_SocketDescriptor = INVALID_SOCKET;
	bool m_IsConnected = false;
	
private:
	static WSADATA s_WsaData;
};