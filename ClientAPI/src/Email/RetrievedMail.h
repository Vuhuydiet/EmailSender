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
	RetrievedMail() = default;
	RetrievedMail(const std::filesystem::path& msg_path);
	RetrievedMail(RetrievedMail&& other) noexcept;
	RetrievedMail(const RetrievedMail& other) = default;
	RetrievedMail& operator=(RetrievedMail&& other) noexcept;
	RetrievedMail& operator=(const RetrievedMail& other) = default;

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