#pragma once

#include "Core/Socket.h"

class ws2Socket : public Socket
{
public:
	virtual void Init(const SocketProps& props) override;

	virtual void Connect(const std::string& ip, int port) override;
	virtual void Disconnect() override;

	virtual void Send(const std::string& msg) override;
	virtual std::string Receive() override;

private:

};