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
	void SetDefaultFolder(const std::string& name);

	const std::set<std::string>& GetAddedMails() const { return m_AddedMails; }
	const std::vector<Ref<RetrievedMail>>& GetRetrievedMails(const std::string& folder_name) const;
	const std::vector<std::pair<std::string, std::vector<Ref<RetrievedMail>>>>& GetRetrievedMails() const { return m_RetrievedMails; }

	MailboxConfig& GetMailboxConfig() { return m_Config; }
private:
	Ref<RetrievedMail> AddNewMail(const std::filesystem::path& msg_path, const std::vector<FilterType>& filters = { FilterType::From, FilterType::Subject, FilterType::Content });
	std::vector<Ref<RetrievedMail>>& GetRefRetrievedMails(const std::string& folder_name);
private:
	std::string m_DefaultFolder = "Default";

	std::vector<std::pair<std::string, std::vector<Ref<RetrievedMail>>>> m_RetrievedMails;
	std::set<std::string> m_AddedMails;

	std::filesystem::path m_MailboxDir;
	MailboxConfig m_Config;
};