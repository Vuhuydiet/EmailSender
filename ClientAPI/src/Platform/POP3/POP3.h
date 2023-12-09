#pragma once

#include "Core/Socket.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <Windows.h>
#include "Email/Library.h"

namespace POP3 {

	// Mail interacting functions
	void LoginServer(Ref<Socket>mail_receiver, const std::string& user_name, const std::string& password);
	void RetreiveMailSFromServer(Ref<Socket>mail_receiver, Library& mail_container, const std::filesystem::path& mailbox_folder_path);
}