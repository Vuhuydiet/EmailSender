#pragma once

#include <string>
#include <vector>
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
			if (!node.IsSequence())
				return false;
			for (auto& item : node) {
				rhs.insert(item.as<std::string>());
			}
			return true;
		}
	};

	template <>
	struct convert<std::pair<std::string, bool>>
	{
		static Node encode(const std::pair<std::string, bool>& rhs)
		{
			Node node;
			node.push_back(rhs.first);
			node.push_back(rhs.second);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, std::pair<std::string, bool>& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;
			
			rhs.first = node[0].as<std::string>();
			rhs.second = node[1].as<bool>();

			return true;
		}
	};

	template <>
	struct convert<std::vector<std::pair<std::string, bool>>>
	{
		static Node encode(const std::vector<std::pair<std::string, bool>>& rhs)
		{
			Node node;
			for (const auto& s : rhs) {
				node.push_back(s);
			}
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, std::vector<std::pair<std::string, bool>>& rhs)
		{
			if (!node.IsSequence())
				return false;
			for (auto& item : node) {
				rhs.push_back(item.as<std::pair<std::string, bool>>());
			}
			return true;
		}
	};
}


inline YAML::Emitter& operator<< (YAML::Emitter& out, const std::set<std::string>& st)
{
	out << YAML::Flow;
	out << YAML::BeginSeq;
	for (const auto& s : st) {
		out << s;
	}
	out << YAML::EndSeq;
	return out;
}

inline YAML::Emitter& operator<< (YAML::Emitter& out, const std::pair<std::string, bool>& st)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << st.first << st.second << YAML::EndSeq;
	return out;
}

inline YAML::Emitter& operator<< (YAML::Emitter& out, const std::vector<std::pair<std::string, bool>>& st)
{
	out << YAML::Flow;
	out << YAML::BeginSeq;
	for (const auto& s : st) {
		out << s;
	}
	out << YAML::EndSeq;
	return out;
}
