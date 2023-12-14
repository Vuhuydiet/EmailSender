#include "app_pch.h"
#include "AutoloadLayer.h"

#include "DefaultVars/Vars.h"
#include "Config/Config.h"

void AutoloadLayer::OnUpdate(float dt) {
	m_TimeCounter += dt;
	
	Config::Get().Load();
	ReloadConfigAttributes();

	if (m_TimeCounter < m_AutoloadDelay || !Config::Get().IsLoggedIn())
		return;

	m_Socket = Socket::Create(SocketProps::AF::INET, SocketProps::Type::SOCKSTREAM, SocketProps::Protocol::IPPROTOCOL_TCP);
	m_Socket->Connect(_SERVER_DEFAULT_IP, _POP3_DEFAULT_PORT);

	POP3::LoginServer(m_Socket, m_Username, m_Password);
	POP3::RetrieveMailsFromServer(m_Socket, _DEFAULT_HOST_MAILBOX_DIR / m_Username);
		
	m_Socket->Disconnect();
	m_Socket = nullptr;
		
	m_TimeCounter = 0.0f;
}

void AutoloadLayer::ReloadConfigAttributes() {
	const auto& config = Config::Get();
	m_AutoloadDelay = config.Autoload();
	m_Username = config.Username();
	m_Password = config.Password();
}

