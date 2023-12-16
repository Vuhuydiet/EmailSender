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

class MailboxConfig {
public:
	void Load(const std::filesystem::path& filter_config_file_path);
	void Save(const std::filesystem::path& filter_config_file_path) const;

	void AddKeyword(std::string keyword, std::string folder_name, FilterType type);
	
	std::vector<std::string> FilterMail(Ref<RetrievedMail> retrieved_mail, const std::vector<FilterType>& filter_types) const;

	void SetReadStatus(const std::string& id, bool read) { m_ReadMailStatus[id] = read; }
	bool GetReadStatus(const std::string& id) const { return _found(m_ReadMailStatus, id) && m_ReadMailStatus.at(id); }
	bool IsAddedToReadStatus(const std::string& id) { return _found(m_ReadMailStatus, id); }
private:
	void FilterMailByFrom(Ref<RetrievedMail> retrieved_mail, std::vector<std::string>& folders) const;
	void FilterMailBySubject(Ref<RetrievedMail> retrieved_mail, std::vector<std::string>& folders) const;
	void FilterMailByContent(Ref<RetrievedMail> retrieved_mail, std::vector<std::string>& folders) const;
private:
	//		folder_name		keywords
	std::map<std::string, std::set<std::string>> m_From;
	std::map<std::string, std::set<std::string>> m_Subject;
	std::map<std::string, std::set<std::string>> m_Content;

	std::map<std::string, bool> m_ReadMailStatus;
};