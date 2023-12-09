#pragma once

#include "Core/Socket.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <Windows.h>

namespace POP3 {
	Ref<Socket> mail_receiver;

	// Mail interacting functions
	void ConnectServer(const std::string& ip, int port);
	void DisconnectServer();
	void LoginServer(const std::string& user_name, const std::string& password);
	void ListMail();
	std::string FindMailID(int mail_id);
	void RetreiveMail(int number_of_mail, const std::string& mail_file_path); // mail_file_path here means directory not includes mail's filename

	// File interacting functions
	bool HasAttachedFile(const std::string& mail_file_path);
	int CountAttachedFile(const std::string& mail_file_path);
	std::string FindReceivedMail(const std::string& mailFile);
	void SaveAttachedFile(const std::string& savedFile, const std::string& mailFile);
}