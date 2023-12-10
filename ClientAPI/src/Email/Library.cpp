#include <pch.h>

#include "Library.h"

void Library::AddNewMail(const RetrievedMail& retreived_mail) {
	m_RetrievedMails.push_back(retreived_mail);
}

const std::vector<RetrievedMail>& Library::GetRetrievedMails() const {
	return m_RetrievedMails;
}
