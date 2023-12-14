#include <pch.h>

#include "Library.h"

void Library::AddNewMail(const RetrievedMail& retrieved_mail, const std::vector<FilterType>& filter_types) {
	Ref<RetrievedMail> retrieved_mail_ref = CreateRef<RetrievedMail>(retrieved_mail);
	std::vector<std::string> filtered_folders = m_Filter.FilterMail(retrieved_mail_ref, filter_types);
	std::string id = retrieved_mail.Id;
	if (_found(m_AddedMails, id)) 
		return;

	for (const auto& folder : filtered_folders) {
		m_RetrievedMails[folder].push_back(retrieved_mail_ref);
	}
	m_AddedMails.insert(id);
}

std::vector<Ref<RetrievedMail>> Library::GetRetrievedMails(const std::string& folder_name) const {
	auto folderName_Mails = m_RetrievedMails.find(folder_name);
	if (folderName_Mails == m_RetrievedMails.end())
		return std::vector<Ref<RetrievedMail>>();

	return folderName_Mails->second;
}


