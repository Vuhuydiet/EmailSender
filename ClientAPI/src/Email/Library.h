#pragma once

#include "RetrievedMail.h"

class Library {
public:
	void AddNewMail(const RetrievedMail& retreived_mail);
	const std::vector<RetrievedMail>& GetRetrievedMails() const;
private:
	std::vector<RetrievedMail> m_RetrievedMails;
};