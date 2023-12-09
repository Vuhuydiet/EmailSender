#pragma once
#include "Core/Socket.h"

namespace SMTP {
	void SendFile(Ref<Socket> socket, const std::string& file_path);
	bool checkSizeOfFile(std::string path);
}
