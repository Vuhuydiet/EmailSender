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
