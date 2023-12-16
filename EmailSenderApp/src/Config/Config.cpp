#include "app_pch.h"
#include "Config.h"

#include <mutex>
#include <yaml-cpp/yaml.h>

static std::mutex s_LoadMutex;
static std::mutex s_SaveMutex;

void Config::Init(const std::filesystem::path& path) {
	m_ConfigFilePath = path;
	if (!std::filesystem::exists(path))
		Save();
}

void Config::Load() {
	std::lock_guard<std::mutex> guard(s_LoadMutex);

	YAML::Node data = YAML::LoadFile(m_ConfigFilePath.string());

	m_MailServer = data["Mail Server"].as<std::string>();
	m_SMTP_port = data["SMTP port"].as<int>();
	m_POP3_port = data["POP3 port"].as<int>();
	m_Autoload = data["Autoload"].as<float>();
	m_MaxSentFileSize = data["Max Sent File Size"].as<size_t>();

	m_IsLoggedIn = data["Logged In"].as<bool>();
	m_Username = data["Username"].as<std::string>();
	m_Password = data["Password"].as<std::string>();
}

void Config::Save() {
	std::lock_guard<std::mutex> guard(s_SaveMutex);

	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Mail Server" << YAML::Value << m_MailServer;
	out << YAML::Key << "SMTP port" << YAML::Value << m_SMTP_port;
	out << YAML::Key << "POP3 port" << YAML::Value << m_POP3_port;
	out << YAML::Key << "Autoload" << YAML::Value << m_Autoload;
	out << YAML::Key << "Max Sent File Size" << YAML::Value << m_MaxSentFileSize;

	out << YAML::Key << "Logged In" << YAML::Value << m_IsLoggedIn;
	out << YAML::Key << "Username" << YAML::Value << m_Username;
	out << YAML::Key << "Password" << YAML::Value << m_Password;
	out << YAML::EndMap;

	std::ofstream fout(m_ConfigFilePath);
	fout << out.c_str();
	fout.close();
}

void Config::LogIn(const std::string& username, const std::string password)
{
	m_IsLoggedIn = true;
	m_Username = username;
	m_Password = password;
}

void Config::LogOut()
{
	m_IsLoggedIn = false;
	m_Username.clear();
	m_Password.clear();
}
