#include <pch.h>

#include "Library.h"
#include <Debug/Debug.h>

Library::Library(const std::filesystem::path& mailbox_dir)
	: m_MailboxDir(mailbox_dir)
{
	LoadMails();
}

Library::~Library() {
}

void Library::LoadMails() {
	for (auto& item : std::filesystem::directory_iterator(m_MailboxDir)) {
		if (_found(m_AddedMails, item.path().filename().string()))
			continue;
		Ref<RetrievedMail> mail = AddNewMail(item.path());
	}
}

bool Library::MoveMail(const std::string& folder_name, const std::string& id, const std::string& destination_folder_name) {
	
	auto& root_folder = GetRefRetrievedMails(folder_name);
	
	if (root_folder.empty()) 
		return false;

	int mail_pos = -1;
	for (int i = 0; i < root_folder.size(); i++) {
		if (root_folder[i]->Id == id) {
			mail_pos = i;
			break;
		}
	}

	if (mail_pos == -1) {
		return false;
	}
	auto mail_temp = root_folder[mail_pos];
	root_folder.erase(root_folder.begin() + mail_pos);

	auto& destination_folder = GetRefRetrievedMails(destination_folder_name);
	destination_folder.push_back(mail_temp);
}

Ref<RetrievedMail> Library::AddNewMail(const std::filesystem::path& msg_path, const std::vector<FilterType>& filters) {
	if (msg_path.extension() != ".msg")
		return nullptr;
	Ref<RetrievedMail> retrieved_mail = CreateRef<RetrievedMail>(msg_path);
	if (_found(m_AddedMails, retrieved_mail->Id)) 
		return nullptr;
	if (m_RetrievedMails.empty())
		m_RetrievedMails.push_back(std::make_pair(m_DefaultFolder, std::vector<Ref<RetrievedMail>>()));
	std::vector<std::string> filtered_folders = m_Config.FilterMail(retrieved_mail, filters);

	for (const auto& folder : filtered_folders) {
		
		auto& found_folder = GetRefRetrievedMails(folder);

		found_folder.push_back(retrieved_mail);
	}

	if (filtered_folders.empty()) {
		auto& found_folder = GetRefRetrievedMails(m_DefaultFolder);

		found_folder.push_back(retrieved_mail);
	}

	m_AddedMails.insert(retrieved_mail->Id);

	if (!m_Config.IsAddedToReadStatus(retrieved_mail->Id)) {
		m_Config.SetReadStatus(retrieved_mail->Id, false);
	}

	return retrieved_mail;
}

void Library::SetDefaultFolder(const std::string& name) {
	if (m_DefaultFolder == "Default") {

		int i = 0;
		while (i < m_RetrievedMails.size() && m_RetrievedMails[i].first != "Default") {
			i++;
		}

		auto& new_default_folder = GetRefRetrievedMails(name);
		if (i < m_RetrievedMails.size()) {
			for (int j = 0; j < m_RetrievedMails[i].second.size(); j++) {
				new_default_folder.push_back(m_RetrievedMails[i].second[j]);
			}
			m_RetrievedMails.erase(m_RetrievedMails.begin() + i);
		}
	}
	m_DefaultFolder = name;
}

void Library::CreateFolder(const std::string& folder) {
	m_RetrievedMails.push_back(std::make_pair(folder, std::vector<Ref<RetrievedMail>>()));
}

const std::vector<Ref<RetrievedMail>>& Library::GetRetrievedMails(const std::string& folder_name) const {
	int folder_name_pos = -1;
	for (int i = 0; i < m_RetrievedMails.size(); i++) {
		if (m_RetrievedMails[i].first == folder_name) {
			folder_name_pos = i;
			break;
		}
	}
	
	if (folder_name_pos == -1) {
		static const std::vector<Ref<RetrievedMail>> s_empty;
		return s_empty;
	}

	return	m_RetrievedMails[folder_name_pos].second;
}

std::vector<Ref<RetrievedMail>>& Library::GetRefRetrievedMails(const std::string& folder_name) {
	int folder_name_pos = -1;
	for (int i = 0; i < m_RetrievedMails.size(); i++) {
		if (m_RetrievedMails[i].first == folder_name) {
			folder_name_pos = i;
			break;
		}
	}

	if (folder_name_pos == -1) {
		static std::vector<Ref<RetrievedMail>> s_empty;
		return s_empty;
	}

	return	m_RetrievedMails[folder_name_pos].second;
}
