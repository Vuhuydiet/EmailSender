#include "pch.h"
#include "FilterConfig.h"

std::vector<std::string> FilterConfig::FilterMailByFrom(Ref<RetrievedMail> retrieved_mail) const {
	std::vector<std::string> folders;
	for (const auto& folder_keywords : m_From) {
		if (_found(folder_keywords.second, retrieved_mail->Sender)) {
			folders.push_back(folder_keywords.first);
		}
	}
	return folders;
}

std::vector<std::string> FilterConfig::FilterMailBySubject(Ref<RetrievedMail> retrieved_mail) const {
	std::vector<std::string> folders;
	for (const auto& folder_keywords : m_Subject) {
		if (_found(folder_keywords.second, retrieved_mail->Subject)) {
			folders.push_back(folder_keywords.first);
		}
	}
	return folders;
}

std::vector<std::string> FilterConfig::FilterMailByContent(Ref<RetrievedMail> retrieved_mail) const {
	std::vector<std::string> folders;
	for (const auto& folder_keywords : m_Content) {
		for (const auto& keyword : folder_keywords.second) {
			if (retrieved_mail->Content.find(keyword) != std::string::npos) {
				folders.push_back(folder_keywords.first);
				break;
			}
		}
	}
	return folders;
}

std::vector<std::string> FilterConfig::FilterMail(Ref<RetrievedMail> retrieved_mail, std::vector<FilterType> filter_types) const {
	for (const auto& type : filter_types) {
		if (type == FilterType::From) {
			return FilterMailByFrom(retrieved_mail);
		}
		else if (type == FilterType::Subject) {
			return FilterMailBySubject(retrieved_mail);
		}
		else {
			return FilterMailByContent(retrieved_mail);
		}
	}

	return std::vector<std::string>();
}


void FilterConfig::AddKeyword(std::string keyword, std::string folder_name, FilterType type) {
	// ? Create new folder if not exist
	
	if (type == FilterType::From) {
		m_From[folder_name].insert(keyword);
	}
	else if (type == FilterType::Subject) {
		m_Subject[folder_name].insert(keyword);
	}
	else {
		m_Content[folder_name].insert(keyword);
	}
}
