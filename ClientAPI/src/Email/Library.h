#pragma once

#include "RetrievedMail.h"
#include "FilterConfig.h"
#include <yaml-cpp/yaml.h>

class Library {
public:
	void AddNewMail(const RetrievedMail& retrieved_mail, const std::vector<FilterType>& filter_types = { FilterType::From, FilterType::Subject, FilterType::Content });
	std::vector<Ref<RetrievedMail>> GetRetrievedMails(const std::string& folder_name) const;
	const std::set<std::string>& GetAddedMails() const { return m_AddedMails; }
	const std::map<std::string, std::vector<Ref<RetrievedMail>>>& GetRetrievedMails() const { return m_RetrievedMails; }
private:
	FilterConfig m_Filter;

	std::map<std::string, std::vector<Ref<RetrievedMail>>> m_RetrievedMails;
	//			id
	std::set<std::string> m_AddedMails;
};