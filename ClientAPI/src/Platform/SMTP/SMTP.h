#pragma once
#include <filesystem>
#include "Core/Socket.h"
#include "Email/SentMail.h"
#include "Core/Date.h"

namespace SMTP {
	void SendMail(Ref<Socket> socket, const SentMail& mail);
	void SendFile(Ref<Socket> socket, const std::filesystem::path& filePath);
	std::string base64_encode(const std::string& input);
}