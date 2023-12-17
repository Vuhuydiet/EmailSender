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

bool Library::MoveMail(const std::string& folder_name, const std::string& id, const std::string& destination_folder) {
	if (_found(m_RetrievedMails, folder_name) && _found(m_RetrievedMails, destination_folder)) {
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
		else {
			return false;
		}
	}

	return false;
}

Ref<RetrievedMail> Library::AddNewMail(const std::filesystem::path& msg_path, const std::vector<FilterType>& filters) {
	if (msg_path.extension() != ".msg")
		return nullptr;
	Ref<RetrievedMail> retrieved_mail = CreateRef<RetrievedMail>(msg_path);
	if (_found(m_AddedMails, retrieved_mail->Id))
		return nullptr;
	std::vector<std::string> filtered_folders = m_Config.FilterMail(retrieved_mail, filters);

	for (const auto& folder : filtered_folders) {
		m_RetrievedMails[folder].push_back(retrieved_mail);
	}
	if (filtered_folders.empty())
		m_RetrievedMails[m_DefaultFolder].push_back(retrieved_mail);

	m_AddedMails.insert(retrieved_mail->Id);

	if (!m_Config.IsAddedToReadStatus(retrieved_mail->Id)) {
		m_Config.SetReadStatus(retrieved_mail->Id, false);
	}

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
	m_RetrievedMails[folder];
	m_AddedFolders.push_back(folder);
}

bool Library::HasFolder(const std::string& folder_name) const {
	if (_found(m_RetrievedMails, folder_name)) return true;

	return false;
}

const std::vector<Ref<RetrievedMail>>& Library::GetRetrievedMails(const std::string& folder_name) const {
	auto folderName_Mails = m_RetrievedMails.find(folder_name);
	if (folderName_Mails == m_RetrievedMails.end()) {
		static const std::vector<Ref<RetrievedMail>> s_empty;
		return s_empty;
	}

	return folderName_Mails->second;
}