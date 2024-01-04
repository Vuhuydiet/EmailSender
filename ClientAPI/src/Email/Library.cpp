#include <pch.h>
#include "Library.h"

#include "Debug/Debug.h"
#include "MailFilter.h"

#include "Utils/yamlConverter.h"

Library::Library(const std::filesystem::path& mailbox_dir)
	: m_MailboxDir(mailbox_dir)
{
}

Library::~Library() {
}

void Library::LoadMails(const MailFilter& filter) {
	for (auto& item : std::filesystem::directory_iterator(m_MailboxDir)) {
		if (item.path().extension() != ".msg")
			continue;
		if (_found(m_AddedMails, item.path().filename().string()))
			continue;
		Ref<RetrievedMail> mail = AddNewMail(item.path(), filter);
	}
}

bool Library::MoveMail(const std::string& folder_name, const std::string& id, const std::string& destination_folder) {
	if (!_found(m_RetrievedMails, folder_name) || !_found(m_RetrievedMails, destination_folder))
		return false;
	
	int mail_pos = 0;
	auto& root_folder = m_RetrievedMails[folder_name];
	while (mail_pos < root_folder.size() && root_folder[mail_pos]->Id != id) {
		mail_pos++;
	}

	if (mail_pos < root_folder.size()) {
		m_RetrievedMails[destination_folder].push_back(root_folder[mail_pos]);
		root_folder.erase(root_folder.begin() + mail_pos);
		return true;
	}

	return false;
}

Ref<RetrievedMail> Library::AddNewMail(const std::filesystem::path& msg_path, const MailFilter& filter) {
	Ref<RetrievedMail> retrieved_mail = CreateRef<RetrievedMail>(msg_path);
	//std::set<std::string> filtered_folders = filter.FilterMail(retrieved_mail);

	//for (const auto& folder : filtered_folders) {
	//	m_RetrievedMails[folder].push_back(retrieved_mail);
	//}
	//if (filtered_folders.empty())
	//	m_RetrievedMails[m_DefaultFolder].push_back(retrieved_mail);

	std::string addedFolder = m_DefaultFolder;
	for (const auto& folder : m_FolderAddedOrders) {
		if (filter.IsFilteredTo(retrieved_mail, folder, { FilterType::From, FilterType::Subject, FilterType::Content })) {
			addedFolder = folder;
			break;
		}
	}

	m_RetrievedMails[addedFolder].push_back(retrieved_mail);
	m_AddedMails.insert(retrieved_mail->Id);
	SetReadStatus(retrieved_mail->Id, false);

	return retrieved_mail;
}

void Library::SetDefaultFolder(const std::string& name) {
	if (m_DefaultFolder == "Default") {
		for (auto& mail : m_RetrievedMails["Default"]) {
			m_RetrievedMails[name].push_back(mail);
		}
		m_RetrievedMails.erase("Default");
	}
	m_DefaultFolder = name;
}

void Library::CreateFolder(const std::string& folder) {
	m_RetrievedMails[folder] = std::vector<Ref<RetrievedMail>>();
	m_FolderAddedOrders.push_back(folder);
}

bool Library::HasFolder(const std::string& folder_name) const {
	return _found(m_RetrievedMails, folder_name);
}

const std::vector<Ref<RetrievedMail>>& Library::GetRetrievedMails(const std::string& folder_name) const {
	auto folderName_Mails = m_RetrievedMails.find(folder_name);
	if (folderName_Mails == m_RetrievedMails.end()) {
		static const std::vector<Ref<RetrievedMail>> s_empty;
		return s_empty;
	}

	return folderName_Mails->second;
}

void Library::LoadMailboxConfig(const std::filesystem::path& mailbox_config_filepath) {
	if (!std::filesystem::exists(mailbox_config_filepath)) {
		__ERROR("'{}' does not exist!", mailbox_config_filepath);
		return;
	}

	YAML::Node data = YAML::LoadFile(mailbox_config_filepath.string());
	
	m_DefaultFolder = data["Default Folder"].as<std::string>();

	auto added_order = data["Folder Added Order"].as<std::vector<std::string>>();
	for (const auto& folder : added_order) {
		CreateFolder(folder);
	}

	for (auto folder : data["Folders"]) {
		std::string folder_name = folder["Name"].as<std::string>();
		for (auto item : folder["Mails"]) {
			auto [id, read] = item.as<std::pair<std::string, bool>>();
			m_RetrievedMails[folder_name].push_back(CreateRef<RetrievedMail>(m_MailboxDir / id));
			m_AddedMails.insert(id);
			m_ReadMailStatus[id] = read;
		}
	}
}

void Library::SaveMailboxConfig(const std::filesystem::path& mailbox_config_filepath) const {
	YAML::Emitter out;

	out << YAML::BeginMap;

	out << YAML::Key << "Default Folder" << YAML::Value << m_DefaultFolder;
	
	out << YAML::Key << "Folder Added Order" << YAML::Value << m_FolderAddedOrders;
	
	out << YAML::Key << "Folders" << YAML::Value << YAML::BeginSeq;
	for (const auto& folder : m_RetrievedMails) {
		out << YAML::BeginMap;
		out << YAML::Key << "Name" << YAML::Value << folder.first;

		out << YAML::Key << "Mails" << YAML::Value << YAML::BeginSeq;
		for (const auto& mail : folder.second) {
			out << std::make_pair(mail->Id, m_ReadMailStatus.at(mail->Id));
		}
		out << YAML::EndSeq;

		out << YAML::EndMap;
	}
	out << YAML::EndSeq;

	out << YAML::EndMap;

	std::ofstream fout(mailbox_config_filepath, std::ios::out);
	fout << out.c_str();
	fout.close();
}