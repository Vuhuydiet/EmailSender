#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

#include "Core/Date.h"

struct FileInfo {
	std::string FileName;
	std::string Base64_Encoded_Content;
};

struct RetrievedMail {
public:
	RetrievedMail() = default;
	explicit RetrievedMail(const std::filesystem::path& msg_path);
	RetrievedMail(RetrievedMail&& other) noexcept;
	RetrievedMail(const RetrievedMail& other) = default;
	RetrievedMail& operator=(RetrievedMail&& other) noexcept;
	RetrievedMail& operator=(const RetrievedMail& other) = default;

	std::string ToString() const;
	void SaveFile(const std::string& file_name, const std::filesystem::path& dir) const;
	void SaveFile(int id, const std::filesystem::path& dir) const;
	void SaveAllFiles(const std::filesystem::path& dir) const;
public:
	Date SendDate;

	std::string Sender;
	std::vector<std::string> Tos;
	std::vector<std::string> Ccs;

	std::string Id;
	std::string Subject;
	std::string Content;
	std::vector<FileInfo> AttachedFiles;
};