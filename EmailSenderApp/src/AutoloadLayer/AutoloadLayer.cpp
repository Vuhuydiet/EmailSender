#include "app_pch.h"
#include "AutoloadLayer.h"

#include "DefaultVars/Vars.h"
#include "Config/Config.h"

void AutoloadLayer::OnUpdate(float dt) {
	m_TimeCounter += dt;
	
	const auto& config = Config::Get();

	if (!config.IsLoggedIn() || m_TimeCounter < config.Autoload())
		return;

	m_Socket = Socket::Create(SocketProps::AF::INET, SocketProps::Type::SOCKSTREAM, SocketProps::Protocol::IPPROTOCOL_TCP);
	m_Socket->Connect(config.MailServer(), config.POP3_Port());

	if (m_Socket->IsConnected()) {
		POP3::LoginServer(m_Socket, config.Username(), config.Password());
		POP3::RetrieveMailsFromServer(m_Socket, _DEFAULT_HOST_MAILBOX_DIR / config.Username());
	}
	else {
		Application::Get().Restart();
	}
		
	m_Socket->Disconnect();
	m_Socket = nullptr;
		
	m_TimeCounter = 0.0f;
}


