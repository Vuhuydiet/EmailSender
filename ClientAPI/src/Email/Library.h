#pragma once

#include "RetrievedMail.h"

class Library {
public:
	void AddNewMail(const RetrievedMail& retreived_mail);
	const std::vector<RetrievedMail>& GetRetrievedMails() const;
	size_t Size() const { return m_RetrievedMails.size(); }
private:
	std::vector<RetrievedMail> m_RetrievedMails;
};