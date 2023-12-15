#include <pch.h>

#include "Library.h"

void Library::AddNewMail(const RetrievedMail& retrieved_mail, const std::string& folder_name) {
	if (_found(m_AddedMails, retrieved_mail.Id)) 
		return;
	Ref<RetrievedMail> retrieved_mail_ref = CreateRef<RetrievedMail>(retrieved_mail);
	/*std::vector<std::string> filtered_folders = m_Filter.FilterMail(retrieved_mail_ref, filter_types);

	for (const auto& folder : filtered_folders) {
		m_RetrievedMails[folder].push_back(retrieved_mail_ref);
	}*/

	m_AddedMails.insert(retrieved_mail.Id);
	m_RetrievedMails[folder_name].push_back(retrieved_mail_ref);
}

void Library::CreateFolder(const std::string& folder) {
	m_RetrievedMails[folder] = std::vector<Ref<RetrievedMail>>();
}

const std::vector<Ref<RetrievedMail>>& Library::GetRetrievedMails(const std::string& folder_name) const {
	auto folderName_Mails = m_RetrievedMails.find(folder_name);
	if (folderName_Mails == m_RetrievedMails.end()) {
		static const std::vector<Ref<RetrievedMail>> s_empty;
		return s_empty;
	}

	return folderName_Mails->second;
}


