#pragma once


#include <filesystem>
#include <yaml-cpp/yaml.h>

#include "RetrievedMail.h"
#include "MailboxConfig.h"

class Library {
public:
	Library(const std::filesystem::path& mailbox_dir);
	~Library();

	void LoadMails();
	bool MoveMail(const std::string& folder_name, const std::string& id, const std::string& destination_folder);

	void CreateFolder(const std::string& folder);
	bool HasFolder(const std::string& folder_name) const;
	void SetDefaultFolder(const std::string& name);

	const std::set<std::string>& GetAddedMails() const { return m_AddedMails; }
	const std::vector<Ref<RetrievedMail>>& GetRetrievedMails(const std::string& folder_name) const;
	const std::map<std::string, std::vector<Ref<RetrievedMail>>>& GetRetrievedMails() const { return m_RetrievedMails; }
	const std::vector <std::string> GetAddedFolder() const { return m_AddedFolders; }

	MailboxConfig& GetMailboxConfig() { return m_Config; }
private:
	Ref<RetrievedMail> AddNewMail(const std::filesystem::path& msg_path, const std::vector<FilterType>& filters = { FilterType::From, FilterType::Subject, FilterType::Content });
private:
	std::string m_DefaultFolder = "Default";

	std::map<std::string, std::vector<Ref<RetrievedMail>>> m_RetrievedMails;
	std::vector<std::string> m_AddedFolders;
	std::set<std::string> m_AddedMails;

	std::filesystem::path m_MailboxDir;
	MailboxConfig m_Config;
};