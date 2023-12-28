#pragma once

#include <string>
#include <filesystem>
#include <sstream>
inline bool IsNumber(const std::string& s) {
	return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

inline bool IsNumberFromTo(const std::string& s, int beg, int end) {
	if (!IsNumber(s))
		return false;
	int num = std::stoi(s);
	return num >= beg && num <= end;
}

inline bool ContainsOnly(const std::string& s, const std::string& char_set) {
	for (auto c : s) {
		if (char_set.find(c) == std::string::npos)
			return false;
	}
	return true;
}

inline void RemoveChar(std::string& str, char c) {
	str.erase(std::remove_if(str.begin(), str.end(), [c](char cc) -> bool { return cc == c; }), str.end());
}

inline std::vector<std::string> Split(const std::string& str, char c) {
	std::vector<std::string> ret;
	std::stringstream ss(str);
	std::string token;
	while (std::getline(ss, token, c)) {
		ret.push_back(token);
	}
	return ret;
}

#define _to_lower(c) (c | (1 << 5))

inline std::string to_lower(std::string s) {
	for (auto& c : s) {
		c = _to_lower(c);
	}
	return s;
}