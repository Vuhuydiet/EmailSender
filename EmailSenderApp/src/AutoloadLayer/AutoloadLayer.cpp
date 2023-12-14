#include "app_pch.h"
#include "AutoloadLayer.h"

#include "DefaultVars/Vars.h"

bool AutoloadLayer::IsLoggedIn() const {
	return !m_Username.empty();
}

void AutoloadLayer::OnUpdate(float dt) {
	LoadConfigFile(_DEFAULT_APP_CONFIG_DIR);

	m_TimeCounter += dt;
	//std::cout << m_TimeCounter << '\n';
	if (m_TimeCounter >= m_AutoloadDelay) {
		m_Socket = Socket::Create({ SocketProps::AF::INET, SocketProps::Type::SOCKSTREAM, SocketProps::Protocol::IPPROTOCOL_TCP });
		m_Socket->Connect(_SERVER_DEFAULT_IP, _POP3_DEFAULT_PORT);
		POP3::LoginServer(m_Socket, m_Username, m_Password);
		POP3::RetreiveMailsFromServer(m_Socket, m_MessageFolderPath / m_Username);
		m_Socket->Disconnect();
		m_Socket = nullptr;
		m_TimeCounter = 0.0f;
	}
	
}

void AutoloadLayer::LoadConfigFile(const std::filesystem::path& path) {
	m_Username = "ty";
	m_Password = "123";
	m_MessageFolderPath = "MSG";
	m_AutoloadDelay = 10.0f;
}