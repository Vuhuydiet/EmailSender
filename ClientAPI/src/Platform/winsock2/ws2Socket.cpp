#include "pch.h"
#include "WS2Socket.h"

#define _MAX_SIZE_PER_SEND			(72)
#define _MAX_SIZE_PER_RECEIVE		(3LL << 20) // 3MB

#define _RECEIVE_ERROR				"##_receive_error"

static const std::map<SocketProps::AF, int> s_ToWs2Af = 
	{ { SocketProps::AF::NONE, -1 }, { SocketProps::AF::INET, AF_INET } };
static const std::map<SocketProps::Type, int> s_ToWs2Type = 
	{ { SocketProps::Type::NONE, -1 }, { SocketProps::Type::SOCKSTREAM, SOCK_STREAM } };
static const std::map<SocketProps::Protocol, int> s_ToWs2Protocol = 
	{ { SocketProps::Protocol::NONE, -1 }, { SocketProps::Protocol::IPPROTOCOL_TCP, IPPROTO_TCP } };


WSADATA WS2Socket::s_WsaData;
static char* s_ReceiveBuffer = nullptr;
static std::mutex s_BufferMutex;

void WS2Socket::Init() {
	int iResult = WSAStartup(MAKEWORD(2, 2), &s_WsaData);
	ASSERT(iResult == 0, "WSAStartup failed: {0}", iResult);

	s_ReceiveBuffer = new char[_MAX_SIZE_PER_RECEIVE + 1];
}

void WS2Socket::Shutdown() {
	delete[] s_ReceiveBuffer;
	s_ReceiveBuffer = nullptr;
}

WS2Socket::WS2Socket(const SocketProps& props)
	: Socket(props)
{
	CreateNewSocket();
}

WS2Socket::WS2Socket(SocketProps::AF af, SocketProps::Type type, SocketProps::Protocol protocol)
	: Socket(af, type, protocol)
{
	CreateNewSocket();
}

WS2Socket::~WS2Socket() {
	if (m_SocketDescriptor != INVALID_SOCKET)
		Disconnect();
}

void WS2Socket::CreateNewSocket()
{
	m_SocketDescriptor = socket(s_ToWs2Af.at(m_Af), s_ToWs2Type.at(m_Type), s_ToWs2Protocol.at(m_Protocol));

	if (m_SocketDescriptor == INVALID_SOCKET) {
		__ERROR("Socket failed with error: {0}", WSAGetLastError());
		WSACleanup();
		return;
	}
}

void WS2Socket::Delete() {
	closesocket(m_SocketDescriptor);
	WSACleanup();
	m_SocketDescriptor = INVALID_SOCKET;
	m_Af = SocketProps::AF::NONE, m_Type = SocketProps::Type::NONE, m_Protocol = SocketProps::Protocol::NONE;
	m_IsConnected = false;
}

void WS2Socket::Connect(const std::string& ip, int port)
{
	sockaddr_in client_service;
	client_service.sin_family = s_ToWs2Af.at(m_Af);
	client_service.sin_addr.s_addr = inet_addr(ip.c_str());
	client_service.sin_port = htons(port);
	
	int iResult = connect(m_SocketDescriptor, (SOCKADDR*)&client_service, sizeof(client_service));
	if (iResult == SOCKET_ERROR) {
		__ERROR("Connect failed with error: {0}", WSAGetLastError());
		Delete();
		return;
	}
	m_IsConnected = true;
	std::string svResponse = Receive();
}

void WS2Socket::Disconnect()
{
	if (!m_IsConnected) 
		return;
	 
	closesocket(m_SocketDescriptor); 
	m_IsConnected = false;
	m_Af = SocketProps::AF::NONE, m_Type = SocketProps::Type::NONE, m_Protocol = SocketProps::Protocol::NONE;
}

void WS2Socket::Send(const std::string& msg)
{
	ASSERT(m_IsConnected, "Hasn't connected, can not send message!");

	std::string formatted_msg = msg + "\r\n";
	for (int i = 0; i < formatted_msg.size(); i += _MAX_SIZE_PER_SEND) {
		std::string sent_string = formatted_msg.substr(i, _MAX_SIZE_PER_SEND);
		int iResult = send(m_SocketDescriptor, sent_string.c_str(), (int)sent_string.size() * sizeof(char), 0);
		
		if (iResult == SOCKET_ERROR) {
			int error_num = WSAGetLastError();
			__ERROR("Send failed : {0}", error_num);
			Delete();
			__debugbreak();
			break;
		}
	}
}

std::string WS2Socket::Receive(size_t size) {
	ASSERT(m_IsConnected, "Hasn't connected! Can not receive message!");

	std::lock_guard<std::mutex> guard(s_BufferMutex);

	std::string res;
	int received_bytes = 0;
	do {
		int iResult = recv(m_SocketDescriptor, s_ReceiveBuffer, _MAX_SIZE_PER_RECEIVE, 0);
		if (iResult > 0) {
			received_bytes += iResult;
			s_ReceiveBuffer[iResult] = '\0';
			res += s_ReceiveBuffer;
		}
		else if (iResult == 0) {
			__INFO("Connection closing...");
		}
		else {
			__ERROR("Recv failed: {0}", WSAGetLastError());
			Delete();
			return _RECEIVE_ERROR;
		}
	} while (received_bytes < size);

	return res;
}

std::string WS2Socket::Receive(const std::string& back_string) {
	ASSERT(m_IsConnected, "Hasn't connected! Can not receive message!");

	std::lock_guard<std::mutex> guard(s_BufferMutex);

	std::string res;
	do {
		int iResult = recv(m_SocketDescriptor, s_ReceiveBuffer, _MAX_SIZE_PER_RECEIVE, 0);
		if (iResult > 0) {
			s_ReceiveBuffer[iResult] = '\0';
			res += s_ReceiveBuffer;
			std::string sub_string = res.substr(res.size() - back_string.size());
			if (sub_string == back_string) 
				break;
		}
		else if (iResult == 0) {
			__INFO("Connection closing...");
			break;
		}
		else {
			__ERROR("Recv failed: {0}", WSAGetLastError());
			Delete();
			return _RECEIVE_ERROR;
		}
	} while (true);

	return res;
}
