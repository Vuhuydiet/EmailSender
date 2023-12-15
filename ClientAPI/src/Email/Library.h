#pragma once

#include "RetrievedMail.h"
#include "FilterConfig.h"
#include <yaml-cpp/yaml.h>

class Library {
public:
	void AddNewMail(const RetrievedMail& retrieved_mail, const std::string& folder_name);
	void CreateFolder(const std::string& folder);

	const std::set<std::string>& GetAddedMails() const { return m_AddedMails; }
	const std::vector<Ref<RetrievedMail>>& GetRetrievedMails(const std::string& folder_name) const;
	const std::map<std::string, std::vector<Ref<RetrievedMail>>>& GetRetrievedMails() const { return m_RetrievedMails; }

private:
	std::map<std::string, std::vector<Ref<RetrievedMail>>> m_RetrievedMails;
	//			id
	std::set<std::string> m_AddedMails;
};