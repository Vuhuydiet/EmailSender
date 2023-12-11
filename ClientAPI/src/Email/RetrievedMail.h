#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>


struct FileInfo {
	std::string FileName;
	std::string Base64_Encoded_Content;
};

struct RetrievedMail {
public:
	std::string ToString() const;
	void SaveFile(const std::string& file_name, const std::filesystem::path& path) const;
public:
	std::string Sender;
	std::vector<std::string> Tos;
	std::vector<std::string> Ccs;

	std::string Subject;
	std::string Content;
	std::vector<FileInfo> AttachedFiles;
};