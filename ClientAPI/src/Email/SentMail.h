#pragma once

#include <string>
#include <vector>
#include <filesystem>

struct SentMail {
public:
	SentMail() = default;
	SentMail(SentMail&& other) noexcept;
	SentMail(const SentMail& other) noexcept = default;
	SentMail& operator=(SentMail&& other) noexcept;
	SentMail& operator=(const SentMail& other) noexcept = default;

	void AddTo(const std::string& to);
	void AddCc(const std::string& cc);
	void AddBcc(const std::string& bcc);

	void AddContentLine(const std::string& text);
	void AddAttachedFile(const std::filesystem::path& path);

public:
	std::string Sender;
	std::vector<std::string> Tos;
	std::vector<std::string> Ccs;
	std::vector<std::string> Bccs;

	std::string Subject;
	std::vector<std::string> Content;
	std::vector<std::filesystem::path> AttachedFilePaths;

};

