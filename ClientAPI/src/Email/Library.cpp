#include <pch.h>

#include "Library.h"

Library::Library(const std::filesystem::path& mailbox_dir)
	: m_MailboxDir(mailbox_dir)
{
	for (const auto& item : std::filesystem::directory_iterator(m_MailboxDir)) {
		AddNewMail(RetrievedMail(item.path()));
	}
}

Library::~Library() {
}

void Library::LoadMails() {
	for (auto& item : std::filesystem::directory_iterator(m_MailboxDir)) {
		if (_found(m_AddedMails, item.path().filename().string()))
			continue;
		Ref<RetrievedMail> mail = AddNewMail(item.path());
		if (mail && m_Config.IsAddedToReadStatus(mail->Id))
			m_Config.SetReadStatus(mail->Id, false);
	}
}

Ref<RetrievedMail> Library::AddNewMail(const RetrievedMail& retrieved_mail, const std::vector<FilterType>& filters) {
	if (_found(m_AddedMails, retrieved_mail.Id)) 
		return nullptr;
	Ref<RetrievedMail> retrieved_mail_ref = CreateRef<RetrievedMail>(retrieved_mail);
	std::vector<std::string> filtered_folders = m_Config.FilterMail(retrieved_mail_ref, filters);

	for (const auto& folder : filtered_folders) {
		m_RetrievedMails[folder].push_back(retrieved_mail_ref);
	}
	if (filtered_folders.empty())
		m_RetrievedMails[m_DefaultFolder].push_back(retrieved_mail_ref);

	m_AddedMails.insert(retrieved_mail.Id);
	return retrieved_mail_ref;
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
}

const std::vector<Ref<RetrievedMail>>& Library::GetRetrievedMails(const std::string& folder_name) const {
	auto folderName_Mails = m_RetrievedMails.find(folder_name);
	if (folderName_Mails == m_RetrievedMails.end()) {
		static const std::vector<Ref<RetrievedMail>> s_empty;
		return s_empty;
	}

	return folderName_Mails->second;
}


