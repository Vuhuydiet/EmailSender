#include "pch.h"
#include "MailFilter.h"

#include <yaml-cpp/yaml.h>
#include "YAML_Converter/Converter.h"

void MailFilter::Load(const std::filesystem::path& path)
{
	/*std::map<std::string, std::set<std::string>> from = {
		{"Inbox", {"nqt@gmail.com", "ntv@gmail.com", "htty@gmail.com"}},
		{"Important", {"ex@gmail.com", "lover@gmail.com", "mum@gmail.com", "dad@gmail.com"}},
		{"Project", {"team@gmail.com", "member@gmail.com", "nqt@gmail.com"}},
		{"Work", {"htty@gmail.com", "ntv@gmail.com"}},
		{"Spam", {"hack@gmail.com", "qc@gmail.com"}}
	};
	std::map<std::string, std::set<std::string>> subject = {
		{"Inbox", {"Hi", "Hello"}},
		{"Important", {"Important", "Urgent"}},
		{"Project", {"Project", "EmailSender"}},
		{"Work", {"Work", "EmailSender"}},
		{"Spam", {"Hack", "QC", "Ad", "Advertisement"}}
	};
	std::map<std::string, std::set<std::string>> content = {
		{"Inbox", {"make friend", "hello", "hi"}},
		{"Important", {"project", "work", "important", "email sender"}},
		{"Project", {"project", "email sender", "wireshark", "socket"}},
		{"Work", {"DSA", "network", "mmt", "nmlt", "oop"}},
		{"Spam", {"hack", "spam", "advertisement"}}
	};
	m_From = from;
	m_Subject = subject;
	m_Content = content;*/
	if (!std::filesystem::exists(path)) {
		__ERROR("File path '{}' does not exist!", path.string());
		return;
	}

	YAML::Node data = YAML::LoadFile(path.string());

	for (auto filter : data["Filters"]) {
		std::string type = filter["Type"].as<std::string>();
		std::map<std::string, std::set<std::string>>* written_filter = nullptr;
		if (type == "From")
			written_filter = &m_From;
		else if (type == "Subject")
			written_filter = &m_Subject;
		else if (type == "Content")
			written_filter = &m_Content;
		else
			continue;
		for (auto folder : filter["Folders"]) {
			std::string folder_name = folder["Name"].as<std::string>();
			std::set<std::string> keys = folder["Keys"].as<std::set<std::string>>();
			(*written_filter)[folder_name] = keys;
		}
	}

}

static void SerializeFilter(YAML::Emitter& out, const std::map<std::string, std::set<std::string>>& filter, const std::string& type) {
	out << YAML::BeginMap;

	out << YAML::Key << "Type" << YAML::Value << type;
	out << YAML::Key << "Folders" << YAML::Value << YAML::BeginSeq;
	for (const auto& folder_mails : filter) {
		out << YAML::BeginMap;
		out << YAML::Key << "Name" << YAML::Value << folder_mails.first;
		out << YAML::Key << "Keys" << YAML::Value << folder_mails.second;
		out << YAML::EndMap;
	}
	out << YAML::EndSeq;

	out << YAML::EndMap;
}

void MailFilter::Save(const std::filesystem::path& path) const {
	YAML::Emitter out;

	out << YAML::BeginMap;

	out << YAML::Key << "Filters" << YAML::Value << YAML::BeginSeq;
	SerializeFilter(out, m_From, "From");
	SerializeFilter(out, m_Subject, "Subject");
	SerializeFilter(out, m_Content, "Content");
	out << YAML::EndSeq;

	out << YAML::EndMap;

	std::ofstream fout(path);
	fout << out.c_str();
	fout.close();
}

std::vector<std::string> MailFilter::FilterMail(Ref<RetrievedMail> retrieved_mail, const std::vector<FilterType>& filter_types) const {
	std::vector<std::string> ret;
	for (const auto& type : filter_types) {
		switch (type)
		{
		case FilterType::From:		
			FilterMailByFrom(retrieved_mail, ret);
			break;
		case FilterType::Subject:	
			FilterMailBySubject(retrieved_mail, ret);
			break;
		case FilterType::Content:	
			FilterMailByContent(retrieved_mail, ret);
			break;
		}
	}

	return ret;
}

void MailFilter::AddKeyword(std::string keyword, std::string folder_name, FilterType type) {
	// ? Create new folder if not exist
	if (type == FilterType::From)
		m_From[folder_name].insert(keyword);
	else if (type == FilterType::Subject)
		m_Subject[folder_name].insert(keyword);
	else if (type == FilterType::Content)
		m_Content[folder_name].insert(keyword);
}

void MailFilter::FilterMailByFrom(Ref<RetrievedMail> retrieved_mail, std::vector<std::string>& dst_folder) const {
	for (const auto& folder_keywords : m_From) {
		if (_found(folder_keywords.second, retrieved_mail->Sender)) {
			dst_folder.push_back(folder_keywords.first);
		}
	}
}

void MailFilter::FilterMailBySubject(Ref<RetrievedMail> retrieved_mail, std::vector<std::string>& dst_folder) const {
	for (const auto& folder_keywords : m_Subject) {
		if (_found(folder_keywords.second, retrieved_mail->Subject)) {
			dst_folder.push_back(folder_keywords.first);
		}
	}
}

void MailFilter::FilterMailByContent(Ref<RetrievedMail> retrieved_mail, std::vector<std::string>& dst_folder) const {
	for (const auto& folder_keywords : m_Content) {
		for (const auto& keyword : folder_keywords.second) {
			if (retrieved_mail->Content.find(keyword) != std::string::npos) {
				dst_folder.push_back(folder_keywords.first);
				break;
			}
		}
	}
}