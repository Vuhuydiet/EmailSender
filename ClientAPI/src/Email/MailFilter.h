#pragma once

#include <string>
#include <map>
#include <set>
#include <vector>

#include "RetrievedMail.h"
#include "Core/Config.h"

enum class FilterType {
	From, Subject, Content
};

class MailFilter {
public:

	void Load(const std::filesystem::path& filter_config_file_path);
	void Save(const std::filesystem::path& filter_config_file_path) const;

	void AddKeyword(std::string keyword, std::string folder_name, FilterType type);
	
	std::set<std::string> FilterMail(Ref<RetrievedMail> retrieved_mail, const std::vector<FilterType>& filter_types) const;
	bool IsFilteredTo(Ref<RetrievedMail> mail, const std::string& folder, const std::vector<FilterType>& types) const;
private:
	void FilterMailByFrom(Ref<RetrievedMail> retrieved_mail, std::set<std::string>& folders) const;
	void FilterMailBySubject(Ref<RetrievedMail> retrieved_mail, std::set<std::string>& folders) const;
	void FilterMailByContent(Ref<RetrievedMail> retrieved_mail, std::set<std::string>& folders) const;
private:
	//		folder_name		keywords
	std::map<std::string, std::set<std::string>> m_From;
	std::map<std::string, std::set<std::string>> m_Subject;
	std::map<std::string, std::set<std::string>> m_Content;
};