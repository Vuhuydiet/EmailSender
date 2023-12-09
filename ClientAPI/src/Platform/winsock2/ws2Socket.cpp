#include "pch.h"
#include "WS2Socket.h"

WSADATA WS2Socket::s_WsaData;

void WS2Socket::Init() {
	int iResult = WSAStartup(MAKEWORD(2, 2), &s_WsaData);
	ASSERT(iResult == 0, "WSAStartup failed: {0}", iResult);
}

WS2Socket::WS2Socket(const SocketProps& props)
	: Socket(props)
{
	CreateNewSocket();
}

WS2Socket::WS2Socket(int af, int type, int protocol)
	: Socket(af, type, protocol)
{
	CreateNewSocket();
}

void WS2Socket::CreateNewSocket()
{
	m_SocketDescriptor = socket(m_Af, m_Type, m_Protocol);

	if (m_SocketDescriptor == INVALID_SOCKET) {
		__ERROR("Socket failed with error: {0}", WSAGetLastError());
		WSACleanup();
		return;
	}
}

void WS2Socket::Connect(const std::string& ip, int port)
{
	sockaddr_in client_service;
	client_service.sin_family = m_Af;
	client_service.sin_addr.s_addr = inet_addr(ip.c_str());
	client_service.sin_port = htons(port);
	
	int iResult = connect(m_SocketDescriptor, (SOCKADDR*)&client_service, sizeof(client_service));
	if (iResult == SOCKET_ERROR) {
		__ERROR("Connect failed with error: {0}", WSAGetLastError());
		closesocket(m_SocketDescriptor);
		WSACleanup();
		return;
	}
	m_IsConnected = true;
}

void WS2Socket::Disconnect()
{
	if (!m_IsConnected) 
		return;
	 
	closesocket(m_SocketDescriptor); 
	m_IsConnected = false;
	m_Af = -1, m_Type = -1, m_Protocol = -1;
}

void WS2Socket::Send(const std::string& msg)
{
	if (!m_IsConnected) {
		__WARN("Hasn't connected, can not send message!");
		return;
	} 
	std::string formatted_msg = msg + "\r\n";
	int iResult = send(m_SocketDescriptor, formatted_msg.c_str(), (int)formatted_msg.size(), 0);
	if (iResult == SOCKET_ERROR) {
		__ERROR("Send failed : {0}", WSAGetLastError());
		closesocket(m_SocketDescriptor);
		WSACleanup();
	}
	/*else {
		__INFO("Bytes sent: {0}", iResult);
		__INFO("Client: {0}", msg);
	}*/
}

void WS2Socket::Receive(std::string& buffer) {
	if (!m_IsConnected) {
		__WARN("Hasn't connected! Can not receive message!");
		return;
	}
	
	char* c_buffer = new char[100];
	int iResult = recv(m_SocketDescriptor, c_buffer, 100, 0);
	
	if (iResult > 0) {
		c_buffer[iResult] = '\0';

		//__INFO("Bytes received: {0}", iResult);
		//__INFO("Server: {0}", buffer);
	}
	else if (iResult == 0) {
		__INFO("Connection closing...");
	}
	else {
		__ERROR("Recv failed: {0}", WSAGetLastError());
		closesocket(m_SocketDescriptor);
		WSACleanup();
	}
	buffer = c_buffer;
	delete[] c_buffer;
}

