#pragma once


#include <filesystem>
#include <yaml-cpp/yaml.h>

#include "RetrievedMail.h"
#include "MailFilter.h"

class Library {
public:
	Library(const std::filesystem::path& mailbox_dir);
	~Library();

	void LoadMailboxConfig(const std::filesystem::path& mailbox_config_filepath);
	void SaveMailboxConfig(const std::filesystem::path& mailbox_config_filepath) const;
	void LoadMails(const MailFilter& filter);
	bool MoveMail(const std::string& folder_name, const std::string& id, const std::string& destination_folder);

	void CreateFolder(const std::string& folder);
	bool HasFolder(const std::string& folder) const;
	void SetDefaultFolder(const std::string& name);

	const std::set<std::string>& GetAddedMails() const { return m_AddedMails; }
	const std::vector<Ref<RetrievedMail>>& GetRetrievedMails(const std::string& folder_name) const;
	const std::map<std::string, std::vector<Ref<RetrievedMail>>>& GetRetrievedMails() const { return m_RetrievedMails; }
	const std::vector <std::string>& GetAddedFolder() const { return m_FolderAddedOrders; }

	void SetReadStatus(const std::string& id, bool read) { m_ReadMailStatus[id] = read; }
	bool GetReadStatus(const std::string& id) const { return _found(m_ReadMailStatus, id) && m_ReadMailStatus.at(id); }
private:
	Ref<RetrievedMail> AddNewMail(const std::filesystem::path& msg_path, const MailFilter& filter, const std::vector<FilterType>& filters = { FilterType::From, FilterType::Subject, FilterType::Content });
private:
	std::string m_DefaultFolder = "Default";

	std::map<std::string, std::vector<Ref<RetrievedMail>>> m_RetrievedMails;
	std::vector<std::string> m_FolderAddedOrders;
	std::set<std::string> m_AddedMails;

	std::filesystem::path m_MailboxDir;

	std::map<std::string, bool> m_ReadMailStatus;

};