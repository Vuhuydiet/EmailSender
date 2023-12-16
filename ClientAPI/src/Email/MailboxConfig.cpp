#include "pch.h"
#include "MailboxConfig.h"

#include <yaml-cpp/yaml.h>


namespace YAML {

	template <>
	struct convert<std::set<std::string>>
	{
		static Node encode(const std::set<std::string>& rhs)
		{
			Node node;
			for (const auto& s : rhs) {
				node.push_back(s);
			}
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, std::set<std::string>& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;
			for (int i = 0; i < node.size(); i++) {
				rhs.insert(node[i].as<std::string>());
			}
			return true;
		}
	};
}

YAML::Emitter& operator<< (YAML::Emitter& out, const std::set<std::string>& st)
{
	out << YAML::Flow;
	out << YAML::BeginSeq;
	for (const auto& s : st) {
		out << s;
	}
	out << YAML::EndSeq;
	return out;
}


void MailboxConfig::Load(const std::filesystem::path& path)
{
	if (!std::filesystem::exists(path)) {
		Save(path);
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

	m_ReadMailStatus = data["Mail Status"].as<std::map<std::string, bool>>();
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

void MailboxConfig::Save(const std::filesystem::path& path) const {
	YAML::Emitter out;

	out << YAML::BeginMap;

	out << YAML::Key << "Filters" << YAML::Value << YAML::BeginSeq;
	SerializeFilter(out, m_From, "From");
	SerializeFilter(out, m_Subject, "Subject");
	SerializeFilter(out, m_Content, "Content");
	out << YAML::EndSeq;

	out << YAML::Key << "Mail Status" << YAML::Value << m_ReadMailStatus;
	
	out << YAML::EndMap;

	std::ofstream fout(path);
	fout << out.c_str();
	fout.close();
}

std::vector<std::string> MailboxConfig::FilterMail(Ref<RetrievedMail> retrieved_mail, const std::vector<FilterType>& filter_types) const {
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

void MailboxConfig::AddKeyword(std::string keyword, std::string folder_name, FilterType type) {
	// ? Create new folder if not exist
	if (type == FilterType::From)
		m_From[folder_name].insert(keyword);
	else if (type == FilterType::Subject)
		m_Subject[folder_name].insert(keyword);
	else if (type == FilterType::Content)
		m_Content[folder_name].insert(keyword);
}

void MailboxConfig::FilterMailByFrom(Ref<RetrievedMail> retrieved_mail, std::vector<std::string>& dst_folder) const {
	for (const auto& folder_keywords : m_From) {
		if (_found(folder_keywords.second, retrieved_mail->Sender)) {
			dst_folder.push_back(folder_keywords.first);
		}
	}
}

void MailboxConfig::FilterMailBySubject(Ref<RetrievedMail> retrieved_mail, std::vector<std::string>& dst_folder) const {
	for (const auto& folder_keywords : m_Subject) {
		if (_found(folder_keywords.second, retrieved_mail->Subject)) {
			dst_folder.push_back(folder_keywords.first);
		}
	}
}

void MailboxConfig::FilterMailByContent(Ref<RetrievedMail> retrieved_mail, std::vector<std::string>& dst_folder) const {
	for (const auto& folder_keywords : m_Content) {
		for (const auto& keyword : folder_keywords.second) {
			if (retrieved_mail->Content.find(keyword) != std::string::npos) {
				dst_folder.push_back(folder_keywords.first);
				break;
			}
		}
	}
}