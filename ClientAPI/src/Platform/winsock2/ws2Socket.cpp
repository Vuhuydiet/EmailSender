#include "pch.h"
#include "ws2Socket.h"


void ws2Socket::Init(const SocketProps& props)
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		// 
		return;
	}


	m_SocketDescriptor = socket(props.af, props.type, props.protocol);

	if (m_SocketDescriptor == INVALID_SOCKET) {
		//printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
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
		printf("WSAStartup failed: %d\n", iResult);
	}

	sockaddr_in client_service;
	client_service.sin_family = m_Af;
	client_service.sin_addr.s_addr = inet_addr(ip.c_str());
	client_service.sin_port = htons(port);
	
	iResult = connect(m_SocketDescriptor, (SOCKADDR*)&client_service, sizeof(client_service));
	if (iResult == SOCKET_ERROR) {
		printf("connect failed with error: %d\n", WSAGetLastError());
		closesocket(m_SocketDescriptor);
		WSACleanup();
	}
	else {
		m_IsConnected = true;
	}
}

void ws2Socket::Disconnect()
{
	if (m_SocketDescriptor == INVALID_SOCKET) {
		closesocket(m_SocketDescriptor); 
		m_IsConnected = false;
	}
}

void ws2Socket::Send(const std::string& msg)
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
	}

	if (!m_IsConnected) {
		printf("Hasn't connected!");
		return;
	} 
	iResult = send(m_SocketDescriptor, (msg+"\r\n").c_str(), msg.size(), 0);
	if (iResult == SOCKET_ERROR) {
		printf("Send failed : % d\n", WSAGetLastError());
		closesocket(m_SocketDescriptor);
		WSACleanup();
	}
	else {
		printf("Bytes sent: %d\n", iResult);
		printf("Client: %s\n", msg);
	}
}

std::string ws2Socket::Receive()
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
	}

	if (!m_IsConnected) {
		printf("Hasn't connected!");
		return std::string();
	}
	
	char* buffer = new char[100];
	iResult = recv(m_SocketDescriptor, buffer, 100, 0);
	
	if (iResult > 0) {
		buffer[iResult] = '\0';

		printf("Bytes received: %d\n", iResult);
		printf("Server: %s\n", buffer);
	}
	else if (iResult == 0) {
		printf("Connection closing...\n");
	}
	else {
		printf("recv failed: %d\n", WSAGetLastError());
		closesocket(m_SocketDescriptor);
		WSACleanup();
	}
	std::string res = buffer;
	delete[] buffer;
	return res;
}
