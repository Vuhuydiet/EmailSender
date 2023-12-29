#pragma once

#include <filesystem>

#include "DefaultVars/Vars.h"

class Config {
public:
	void Init(const std::filesystem::path& path);
	void Load();
	void Save();
	
	const std::string& MailServer() const { return m_MailServer; }
	int SMTP_Port() const { return m_SMTP_port; }
	int POP3_Port() const { return m_POP3_port; }
	float Autoload() const { return m_Autoload; }
	size_t MaxSentFileSize() const { return m_MaxSentFileSize; }
	const std::string& Username() const { return m_Username; }
	const std::string& Password() const { return m_Password; }

	void SetMailServer(const std::string& sv) { m_MailServer = sv; }
	void SetSMTP_Port(int port) { m_SMTP_port = port; }
	void SetPOP3_Port(int port) { m_POP3_port = port; }
	void SetAutoload(float autoload) { m_Autoload = autoload; }

	void LogIn(const std::string& username, const std::string password);
	void LogOut();
	bool IsLoggedIn() const { return m_IsLoggedIn; }

	static Config& Get() { static Config s_Config; return s_Config; }

private:
	std::string m_MailServer = _SERVER_DEFAULT_IP;
	int m_SMTP_port = _SMTP_DEFAULT_PORT;
	int m_POP3_port = _POP3_DEFAULT_PORT;
	float m_Autoload = 10.0f;
	size_t m_MaxSentFileSize = 3LL << 20; // 3 MB

	bool m_IsLoggedIn = false;
	std::string m_Username;
	std::string m_Password;

	std::filesystem::path m_ConfigFilePath;
};