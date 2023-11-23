#include "pch.h"
#include "ws2Socket.h"

void ws2Socket::Init(const SocketProps& props)
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		_ERROR("WSAStartup failed: {0}", iResult);
		return;
	}

	m_SocketDescriptor = socket(props.af, props.type, props.protocol);

	if (m_SocketDescriptor == INVALID_SOCKET) {
		_ERROR("Socket failed with error: {0}", WSAGetLastError());
		WSACleanup();
		return;
	}
	m_Af = props.af;
	m_Type = props.type;
	m_Protocol = props.protocol;
}

void ws2Socket::Connect(const std::string& ip, int port)
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		_ERROR("WSAStartup failed: {0}", iResult);
		return;
	}

	sockaddr_in client_service;
	client_service.sin_family = m_Af;
	client_service.sin_addr.s_addr = inet_addr(ip.c_str());
	client_service.sin_port = htons(port);
	
	iResult = connect(m_SocketDescriptor, (SOCKADDR*)&client_service, sizeof(client_service));
	if (iResult == SOCKET_ERROR) {
		_ERROR("Connect failed with error: {0}", WSAGetLastError());
		closesocket(m_SocketDescriptor);
		WSACleanup();
		return;
	}
	else {
		m_IsConnected = true;
	}
}

void ws2Socket::Disconnect()
{
	if (!m_IsConnected) 
		return;
	 
	closesocket(m_SocketDescriptor); 
	m_IsConnected = false;
	
}

void ws2Socket::Send(const std::string& msg)
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		_ERROR("WSAStartup failed: {0}", iResult);
		return;
	}

	if (!m_IsConnected) {
		_WARN("Hasn't connected, can not send message!");
		return;
	} 
	iResult = send(m_SocketDescriptor, (msg+"\r\n").c_str(), (int)msg.size(), 0);
	if (iResult == SOCKET_ERROR) {
		_ERROR("Send failed : {0}", WSAGetLastError());
		closesocket(m_SocketDescriptor);
		WSACleanup();
	}
	else {
		_INFO("Bytes sent: {0}", iResult);
		_INFO("Client: {0}", msg);
	}
}

std::string ws2Socket::Receive()
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		_ERROR("WSAStartup failed: {0}", iResult);
		return std::string();
	}

	if (!m_IsConnected) {
		_WARN("Hasn't connected! Can not receive message!");
		return std::string();
	}
	
	char* buffer = new char[100];
	iResult = recv(m_SocketDescriptor, buffer, 100, 0);
	
	if (iResult > 0) {
		buffer[iResult] = '\0';

		_INFO("Bytes received: {0}", iResult);
		_INFO("Server: {0}", buffer);
	}
	else if (iResult == 0) {
		_INFO("Connection closing...");
	}
	else {
		_ERROR("Recv failed: {0}", WSAGetLastError());
		closesocket(m_SocketDescriptor);
		WSACleanup();
	}
	std::string res = buffer;
	delete[] buffer;
	return res;
}
