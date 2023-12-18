#pragma once
#include <filesystem>
#include "Core/Socket.h"
#include "Email/SentMail.h"
#include "Core/Date.h"
#include "Base64/Base64.h"

namespace SMTP {
	bool SendMail(Ref<Socket> socket, const SentMail& mail);
	void SendFile(Ref<Socket> socket, const std::filesystem::path& filePath);
	void EndSession(Ref<Socket> socket);
}